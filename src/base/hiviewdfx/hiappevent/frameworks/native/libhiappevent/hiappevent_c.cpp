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

#include "hiappevent_c.h"

#include <vector>

#include "hiappevent_base.h"
#include "hiappevent_config.h"
#include "hiappevent_verify.h"
#include "hiappevent_write.h"
#include "hilog/log.h"

using namespace OHOS::HiviewDFX;

namespace {
const HiLogLabel LABEL = { LOG_CORE, HIAPPEVENT_DOMAIN, "HiAppEvent_c" };

template<typename T>
void AddArrayParam(std::shared_ptr<AppEventPack>& appEventPack, const char* name, const T* arr, int len)
{
    std::vector<T> params(arr, arr + len);
    appEventPack->AddParam(name, params);
}

using ParamAdder = void (*)(std::shared_ptr<AppEventPack>& appEventPack, const char* name, const ParamValue* value);

void AddBoolParamValue(std::shared_ptr<AppEventPack>& appEventPack, const char* name, const ParamValue* value)
{
    appEventPack->AddParam(name, value->value.bool_v);
}

void AddBoolArrayParamValue(std::shared_ptr<AppEventPack>& appEventPack, const char* name, const ParamValue* value)
{
    AddArrayParam(appEventPack, name, value->value.bool_arr_v, value->arrSize);
}

void AddInt8ParamValue(std::shared_ptr<AppEventPack>& appEventPack, const char* name, const ParamValue* value)
{
    appEventPack->AddParam(name, value->value.int8_v);
}

void AddInt8ArrayParamValue(std::shared_ptr<AppEventPack>& appEventPack, const char* name, const ParamValue* value)
{
    AddArrayParam(appEventPack, name, value->value.int8_arr_v, value->arrSize);
}

void AddInt16ParamValue(std::shared_ptr<AppEventPack>& appEventPack, const char* name, const ParamValue* value)
{
    appEventPack->AddParam(name, value->value.int16_v);
}

void AddInt16ArrayParamValue(std::shared_ptr<AppEventPack>& appEventPack, const char* name, const ParamValue* value)
{
    AddArrayParam(appEventPack, name, value->value.int16_arr_v, value->arrSize);
}

void AddInt32ParamValue(std::shared_ptr<AppEventPack>& appEventPack, const char* name, const ParamValue* value)
{
    appEventPack->AddParam(name, value->value.int32_v);
}

void AddInt32ArrayParamValue(std::shared_ptr<AppEventPack>& appEventPack, const char* name, const ParamValue* value)
{
    AddArrayParam(appEventPack, name, value->value.int32_arr_v, value->arrSize);
}

void AddInt64ParamValue(std::shared_ptr<AppEventPack>& appEventPack, const char* name, const ParamValue* value)
{
    appEventPack->AddParam(name, value->value.int64_v);
}

void AddInt64ArrayParamValue(std::shared_ptr<AppEventPack>& appEventPack, const char* name, const ParamValue* value)
{
    AddArrayParam(appEventPack, name, value->value.int64_arr_v, value->arrSize);
}

void AddFloatParamValue(std::shared_ptr<AppEventPack>& appEventPack, const char* name, const ParamValue* value)
{
    appEventPack->AddParam(name, value->value.float_v);
}

void AddFloatArrayParamValue(std::shared_ptr<AppEventPack>& appEventPack, const char* name, const ParamValue* value)
{
    AddArrayParam(appEventPack, name, value->value.float_arr_v, value->arrSize);
}

void AddDoubleParamValue(std::shared_ptr<AppEventPack>& appEventPack, const char* name, const ParamValue* value)
{
    appEventPack->AddParam(name, value->value.double_v);
}

void AddDoubleArrayParamValue(std::shared_ptr<AppEventPack>& appEventPack, const char* name, const ParamValue* value)
{
    AddArrayParam(appEventPack, name, value->value.double_arr_v, value->arrSize);
}

void AddStringParamValue(std::shared_ptr<AppEventPack>& appEventPack, const char* name, const ParamValue* value)
{
    appEventPack->AddParam(name, value->value.str_v);
}

void AddStringArrayParamValue(std::shared_ptr<AppEventPack>& appEventPack, const char* name, const ParamValue* value)
{
    AddArrayParam(appEventPack, name, value->value.str_arr_v, value->arrSize);
}

const ParamAdder adders[] = {
    &AddBoolParamValue,
    &AddBoolArrayParamValue,
    &AddInt8ParamValue,
    &AddInt8ArrayParamValue,
    &AddInt16ParamValue,
    &AddInt16ArrayParamValue,
    &AddInt32ParamValue,
    &AddInt32ArrayParamValue,
    &AddInt64ParamValue,
    &AddInt64ArrayParamValue,
    &AddFloatParamValue,
    &AddFloatArrayParamValue,
    &AddDoubleParamValue,
    &AddDoubleArrayParamValue,
    &AddStringParamValue,
    &AddStringArrayParamValue
};

void AddParamValue(std::shared_ptr<AppEventPack>& appEventPack, const char* name, const ParamValue* value)
{
    if (name == nullptr || value == nullptr) {
        HiLog::Error(LABEL, "Failed to add the param because the name or value is null.");
        return;
    }
    unsigned int paramType = value->type;
    if (paramType < (sizeof(adders) / sizeof(adders[0]))) {
        adders[paramType](appEventPack, name, value);
    } else {
        HiLog::Error(LABEL, "Failed to add the param because the param type is unknown.");
    }
}

void AddParamEntry(std::shared_ptr<AppEventPack>& appEventPack, const ParamEntry* entry)
{
    if (entry == nullptr) {
        HiLog::Error(LABEL, "Failed to add the param because the entry is null.");
        return;
    }
    AddParamValue(appEventPack, entry->name, entry->value);
}

void AddParamList(std::shared_ptr<AppEventPack>& appEventPack, const ParamList list)
{
    ParamList curNode = list;
    while (curNode != nullptr) {
        AddParamEntry(appEventPack, curNode->entry);
        curNode = curNode->next;
    }
}
}

bool HiAppEventInnerConfigure(const char* name, const char* value)
{
    if (name == nullptr || value == nullptr) {
        HiLog::Error(LABEL, "Failed to configure, because the input params contains a null pointer.");
        return false;
    }
    return HiAppEventConfig::GetInstance().SetConfigurationItem(name, value);
}

int HiAppEventInnerWrite(const char* domain, const char* name, EventType type, const ParamList list)
{
    if (domain == nullptr || name == nullptr) {
        HiLog::Error(LABEL, "Failed to write event, because the input params contains a null pointer.");
        return ErrorCode::ERROR_INVALID_EVENT_NAME;
    }

    std::shared_ptr<AppEventPack> appEventPack = std::make_shared<AppEventPack>(name, type);
    AddParamList(appEventPack, list);
    int res = VerifyAppEvent(appEventPack);
    if (res >= 0) {
        WriterEvent(appEventPack);
    }
    return res;
}
