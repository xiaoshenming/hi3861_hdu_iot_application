/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "hisysevent.h"

#include <chrono>
#include <iomanip>
#include <sstream>
#include <sys/time.h>
#include <unistd.h>

#include "def.h"
#include "hilog/log.h"
#include "hitrace/hitraceid.h"
#include "hitrace/trace.h"
#include "stringfilter.h"
#include "transport.h"

namespace OHOS {
namespace HiviewDFX {
namespace {
constexpr HiLogLabel LABEL = { LOG_CORE, 0xD002D08, "HISYSEVENT" };
constexpr int SECS_IN_MINUTE = 60;
constexpr int SECS_IN_HOUR = 3600;
}

static inline uint64_t GetMilliseconds()
{
    auto now = std::chrono::system_clock::now();
    auto millisecs = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return millisecs.count();
}

static std::string GetTimeZone()
{
    struct timeval tv;
    if (gettimeofday(&tv, NULL) != 0) {
        HiLog::Error(LABEL, "can not get tz");
        return "";
    }
    time_t sysSec = tv.tv_sec;
    struct tm tmLocal;
    if (localtime_r(&sysSec, &tmLocal) == nullptr) {
        HiLog::Error(LABEL, "failed to get local time.");
        return "";
    }
    struct tm tmUtc;
    if (gmtime_r(&sysSec, &tmUtc) == nullptr) {
        HiLog::Error(LABEL, "failed to get GMT time.");
        return "";
    }
    time_t diffSec = mktime(&tmLocal) - mktime(&tmUtc);
    int tzHour = std::abs(diffSec) / SECS_IN_HOUR;
    if (tzHour > 12) { // max time zone is 12
        HiLog::Error(LABEL, "failed to get hours for time zone, set to 0.");
        tzHour = 0;
    }
    int tzMin = (std::abs(diffSec) % SECS_IN_HOUR) / SECS_IN_MINUTE;
    std::stringstream ss;
    ss << ((diffSec < 0) ? "-" : "+");
    ss << std::setw(2) << std::setfill('0') << tzHour; // the number of digits in the hour is 2
    ss << std::setw(2) << std::setfill('0') << tzMin; // the number of digits in the min is 2
    return ss.str();
}

int HiSysEvent::CheckKey(const std::string &key)
{
    if (!StringFilter::GetInstance().IsValidName(key, MAX_PARAM_NAME_LENGTH)) {
        return ERR_KEY_NAME_INVALID;
    }
    return SUCCESS;
}

int HiSysEvent::CheckValue(const std::string &value)
{
    if (value.length() > MAX_STRING_LENGTH) {
        return ERR_VALUE_LENGTH_TOO_LONG;
    }
    return SUCCESS;
}

int HiSysEvent::CheckArraySize(unsigned long size)
{
    if (size > MAX_ARRAY_SIZE) {
        return ERR_ARRAY_TOO_MUCH;
    }
    return SUCCESS;
}

unsigned int HiSysEvent::GetArrayMax()
{
    return MAX_ARRAY_SIZE;
}

void HiSysEvent::ExplainRetCode(HiSysEvent::EventBase &eventBase)
{
    if (eventBase.retCode_ > SUCCESS) {
        HiLog::Warn(LABEL, "some value of param discard as invalid data, error=%{public}d, message=%{public}s",
            eventBase.retCode_, ERR_MSG_LEVEL1[eventBase.retCode_ - 1]);
    } else if (eventBase.retCode_ < SUCCESS) {
        HiLog::Error(LABEL, "discard data, error=%{public}d, message=%{public}s",
            eventBase.retCode_, ERR_MSG_LEVEL0[-eventBase.retCode_ - 1]);
    }
}

bool HiSysEvent::IsError(HiSysEvent::EventBase &eventBase)
{
    return (eventBase.retCode_ < SUCCESS);
}

bool HiSysEvent::IsErrorAndUpdate(int retCode, HiSysEvent::EventBase &eventBase)
{
    if (retCode < SUCCESS) {
        eventBase.retCode_ = retCode;
        return true;
    }
    return false;
}

bool HiSysEvent::IsWarnAndUpdate(int retCode, EventBase &eventBase)
{
    if (retCode != SUCCESS) {
        eventBase.retCode_ = retCode;
        return true;
    }
    return false;
}

bool HiSysEvent::UpdateAndCheckKeyNumIsOver(HiSysEvent::EventBase &eventBase)
{
    eventBase.keyCnt_++;
    if (eventBase.keyCnt_ > MAX_PARAM_NUMBER) {
        eventBase.retCode_ = ERR_KEY_NUMBER_TOO_MUCH;
        return true;
    }
    return false;
}

void HiSysEvent::AppendValue(HiSysEvent::EventBase &eventBase, const std::string &item)
{
    std::string text = item;
    if (item.length() > MAX_STRING_LENGTH) {
        text = item.substr(0, MAX_STRING_LENGTH);
        eventBase.retCode_ = ERR_VALUE_LENGTH_TOO_LONG;
    }
    eventBase.jsonStr_ << "\"" << StringFilter::GetInstance().EscapeToRaw(text) << "\"";
}

void HiSysEvent::AppendValue(HiSysEvent::EventBase &eventBase, const char item)
{
    eventBase.jsonStr_ << static_cast<short>(item);
}

void HiSysEvent::AppendValue(HiSysEvent::EventBase &eventBase, const unsigned char item)
{
    eventBase.jsonStr_ << static_cast<unsigned short>(item);
}

void HiSysEvent::InnerWrite(HiSysEvent::EventBase &eventBase)
{
    if (eventBase.jsonStr_.tellp() != 0) {
        eventBase.jsonStr_.seekp(-1, std::ios_base::end);
    }
}

void HiSysEvent::SendSysEvent(HiSysEvent::EventBase &eventBase)
{
    int r = Transport::GetInstance().SendData(eventBase.jsonStr_.str());
    if (r != SUCCESS) {
        eventBase.retCode_ = r;
        ExplainRetCode(eventBase);
    }
}

void HiSysEvent::AppendHexData(HiSysEvent::EventBase &eventBase, const std::string &key, uint64_t value)
{
    eventBase.jsonStr_ << "\"" << key << "\":\"" << std::hex << value << "\"," << std::dec;
}

void HiSysEvent::WritebaseInfo(HiSysEvent::EventBase &eventBase)
{
    if (!StringFilter::GetInstance().IsValidName(eventBase.domain_, MAX_DOMAIN_LENGTH)) {
        eventBase.retCode_ = ERR_DOMAIN_NAME_INVALID;
        return;
    }
    if (!StringFilter::GetInstance().IsValidName(eventBase.eventName_, MAX_EVENT_NAME_LENGTH)) {
        eventBase.retCode_ = ERR_EVENT_NAME_INVALID;
        return;
    }
    AppendData(eventBase, "domain_", eventBase.domain_);
    AppendData(eventBase, "name_", eventBase.eventName_);
    AppendData(eventBase, "type_", eventBase.type_);
    AppendData(eventBase, "time_", GetMilliseconds());
    AppendData(eventBase, "tz_", GetTimeZone());
    AppendData(eventBase, "pid_", getpid());
    AppendData(eventBase, "tid_", gettid());
    AppendData(eventBase, "uid_", getuid());
    HiTraceId hitraceId = HiTrace::GetId();
    if (!hitraceId.IsValid()) {
        eventBase.keyCnt_ = 0;
        return;
    }
    AppendHexData(eventBase, "traceid_", hitraceId.GetChainId());
    AppendHexData(eventBase, "spanid_", hitraceId.GetSpanId());
    AppendHexData(eventBase, "pspanid_", hitraceId.GetParentSpanId());
    AppendData(eventBase, "trace_flag_", hitraceId.GetFlags());
    eventBase.keyCnt_ = 0;
}
} // namespace HiviewDFX
} // namespace OHOS

