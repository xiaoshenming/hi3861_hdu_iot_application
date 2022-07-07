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

#include "napi_hitrace_native_call_wrapper.h"

#include <string>

#include "hilog/log.h"
#include "hitrace/trace.h"
#include "napi_hitrace_param.h"
#include "napi_hitrace_util.h"

namespace OHOS {
namespace HiviewDFX {
namespace {
constexpr HiLogLabel LABEL = { LOG_CORE, 0xD002D03, "NapiHiTraceNativeCallWrapper" };
constexpr uint32_t BUF_SIZE_64 = 64;
}
void NapiHiTraceNativeCallWrapper::Begin(napi_env env, size_t paramNum, napi_value* params,
    napi_value& result)
{
    if (params == nullptr) {
        return;
    }
    if (!NapiHitraceUtil::NapiTypeCheck(env, params[ParamIndex::PARAM_FIRST],
        napi_valuetype::napi_string)) {
        HiLog::Error(LABEL, "name type must be string.");
        return;
    }
    char name[BUF_SIZE_64];
    size_t nameLength;
    napi_get_value_string_utf8(env, params[ParamIndex::PARAM_FIRST], name,
        BUF_SIZE_64, &nameLength);
    HiTraceId traceId;
    if (paramNum == ParamNum::TOTAL_ONE) {
        traceId = HiTrace::Begin(name, HiTraceFlag::HITRACE_FLAG_DEFAULT);
    } else if (paramNum == ParamNum::TOTAL_TWO) {
        if (!NapiHitraceUtil::NapiTypeCheck(env, params[ParamIndex::PARAM_SECOND],
            napi_valuetype::napi_number)) {
            HiLog::Error(LABEL, "flag param type must be number.");
            return;
        }
        int flag;
        napi_get_value_int32(env, params[ParamIndex::PARAM_SECOND], &flag);
        traceId = HiTrace::Begin(name, flag);
    }
    NapiHitraceUtil::CreateHiTraceIdObject(env, traceId, result);
}

void NapiHiTraceNativeCallWrapper::End(napi_env env, napi_value* params, napi_value& result)
{
    if (params == nullptr) {
        return;
    }
    if (!NapiHitraceUtil::NapiTypeCheck(env, params[ParamIndex::PARAM_FIRST],
        napi_valuetype::napi_object)) {
        HiLog::Error(LABEL, "hitarce id type must be object.");
        return;
    }
    HiTraceId traceId;
    NapiHitraceUtil::TransHiTraceIdObjectToNative(env, traceId, params[ParamIndex::PARAM_FIRST]);
    HiTrace::End(traceId);
}

void NapiHiTraceNativeCallWrapper::GetId(napi_env env, napi_value& result)
{
    HiTraceId traceId = HiTrace::GetId();
    NapiHitraceUtil::CreateHiTraceIdObject(env, traceId, result);
}

void NapiHiTraceNativeCallWrapper::SetId(napi_env env, napi_value* params, napi_value& result)
{
    if (params == nullptr) {
        return;
    }
    if (!NapiHitraceUtil::NapiTypeCheck(env, params[ParamIndex::PARAM_FIRST],
        napi_valuetype::napi_object)) {
        HiLog::Error(LABEL, "hitarce id type must be object.");
        return;
    }
    HiTraceId traceId;
    NapiHitraceUtil::TransHiTraceIdObjectToNative(env, traceId, params[ParamIndex::PARAM_FIRST]);
    HiTrace::SetId(traceId);
}

void NapiHiTraceNativeCallWrapper::ClearId(napi_env env, napi_value& result)
{
    HiTrace::ClearId();
}

void NapiHiTraceNativeCallWrapper::CreateSpan(napi_env env, napi_value& result)
{
    HiTraceId traceId = HiTrace::CreateSpan();
    NapiHitraceUtil::CreateHiTraceIdObject(env, traceId, result);
}

void NapiHiTraceNativeCallWrapper::Tracepoint(napi_env env, napi_value* params, napi_value& result)
{
    if (params == nullptr) {
        return;
    }
    if (!NapiHitraceUtil::NapiTypeCheck(env, params[ParamIndex::PARAM_FIRST],
        napi_valuetype::napi_number)) {
        HiLog::Error(LABEL, "HiTraceCommunicationMode type must be number.");
        return;
    }
    int communicationModeInt = 0;
    napi_get_value_int32(env, params[ParamIndex::PARAM_FIRST], &communicationModeInt);
    HiTraceCommunicationMode communicationMode = HiTraceCommunicationMode(communicationModeInt);
    if (!NapiHitraceUtil::NapiTypeCheck(env, params[ParamIndex::PARAM_SECOND],
        napi_valuetype::napi_number)) {
        HiLog::Error(LABEL, "HiTraceTracePointType type must be number.");
        return;
    }
    int tracePointTypeInt = 0;
    napi_get_value_int32(env, params[ParamIndex::PARAM_SECOND], &tracePointTypeInt);
    HiTraceTracepointType tracePointType = HiTraceTracepointType(tracePointTypeInt);
    if (!NapiHitraceUtil::NapiTypeCheck(env, params[ParamIndex::PARAM_THIRD],
        napi_valuetype::napi_object)) {
        HiLog::Error(LABEL, "hitarce id type must be object.");
        return;
    }
    HiTraceId traceId;
    NapiHitraceUtil::TransHiTraceIdObjectToNative(env, traceId, params[ParamIndex::PARAM_THIRD]);
    if (!NapiHitraceUtil::NapiTypeCheck(env, params[ParamIndex::PARAM_FORTH],
        napi_valuetype::napi_string)) {
        HiLog::Error(LABEL, "descriptione must be string.");
        return;
    }
    char descprition[BUF_SIZE_64];
    size_t descLength;
    napi_get_value_string_utf8(env, params[ParamIndex::PARAM_FORTH], descprition,
        BUF_SIZE_64, &descLength);
    HiTrace::Tracepoint(communicationMode, tracePointType, traceId, "%s", descprition);
}

void NapiHiTraceNativeCallWrapper::IsValid(napi_env env, napi_value* params, napi_value& result)
{
    if (params == nullptr) {
        return;
    }
    if (!NapiHitraceUtil::NapiTypeCheck(env, params[ParamIndex::PARAM_FIRST],
        napi_valuetype::napi_object)) {
        HiLog::Error(LABEL, "hitarce id type must be object.");
        return;
    }
    HiTraceId traceId;
    NapiHitraceUtil::TransHiTraceIdObjectToNative(env, traceId, params[ParamIndex::PARAM_FIRST]);
    bool isValid = traceId.IsValid();
    napi_get_boolean(env, isValid, &result);
}

void NapiHiTraceNativeCallWrapper::IsFlagEnabled(napi_env env, napi_value* params, napi_value& result)
{
    if (params == nullptr) {
        return;
    }
    if (!NapiHitraceUtil::NapiTypeCheck(env, params[ParamIndex::PARAM_FIRST],
        napi_valuetype::napi_object)) {
        HiLog::Error(LABEL, "hitarce id type must be object.");
        return;
    }
    HiTraceId traceId;
    NapiHitraceUtil::TransHiTraceIdObjectToNative(env, traceId, params[ParamIndex::PARAM_FIRST]);
    if (!NapiHitraceUtil::NapiTypeCheck(env, params[ParamIndex::PARAM_SECOND],
        napi_valuetype::napi_number)) {
        HiLog::Error(LABEL, "HiTraceFlag must be number.");
        return;
    }
    int traceFlagInt = 0;
    napi_get_value_int32(env, params[ParamIndex::PARAM_SECOND], &traceFlagInt);
    HiTraceFlag traceFlag = HiTraceFlag(traceFlagInt);
    bool isFalgEnabled = traceId.IsFlagEnabled(traceFlag);
    napi_get_boolean(env, isFalgEnabled, &result);
}

void NapiHiTraceNativeCallWrapper::EnableFlag(napi_env env, napi_value* params, napi_value& result)
{
    if (params == nullptr) {
        return;
    }
    if (!NapiHitraceUtil::NapiTypeCheck(env, params[ParamIndex::PARAM_FIRST],
        napi_valuetype::napi_object)) {
        HiLog::Error(LABEL, "hitarce id type is not object.");
        return;
    }
    HiTraceId traceId;
    NapiHitraceUtil::TransHiTraceIdObjectToNative(env, traceId, params[ParamIndex::PARAM_FIRST]);
    if (!NapiHitraceUtil::NapiTypeCheck(env, params[ParamIndex::PARAM_SECOND],
        napi_valuetype::napi_number)) {
        HiLog::Error(LABEL, "HiTraceFlag must be number.");
        return;
    }
    int traceFlagInt = 0;
    napi_get_value_int32(env, params[ParamIndex::PARAM_SECOND], &traceFlagInt);
    HiTraceFlag traceFlag = HiTraceFlag(traceFlagInt);
    traceId.EnableFlag(traceFlag);
    NapiHitraceUtil::EnableTraceIdObjectFlag(env, traceId, params[ParamIndex::PARAM_FIRST]);
}
} // namespace HiviewDFX
} // namespace OHOS
