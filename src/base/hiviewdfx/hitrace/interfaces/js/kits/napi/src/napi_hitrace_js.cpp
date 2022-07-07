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

#include "napi_hitrace_init.h"

#include "hilog/log.h"
#include "napi_hitrace_param.h"
#include "napi_hitrace_util.h"
#include "napi_hitrace_native_call_wrapper.h"

using namespace OHOS::HiviewDFX;

namespace {
    constexpr HiLogLabel LABEL = { LOG_CORE, 0xD002D03, "HiTrace_NAPI" };
}

static napi_value Begin(napi_env env, napi_callback_info info)
{
    size_t paramNum = ParamNum::TOTAL_TWO;
    napi_value params[ParamNum::TOTAL_TWO] = {0};
    napi_value thisArg = nullptr;
    void* data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &paramNum, params, &thisArg, &data));

    napi_value result = nullptr;
    if (paramNum != ParamNum::TOTAL_ONE && paramNum != ParamNum::TOTAL_TWO) {
        HiLog::Error(LABEL,
            "failed to begin a new trace, count of parameters is invalid.");
        return result;
    }
    NapiHiTraceNativeCallWrapper::Begin(env, paramNum, params, result);
    return result;
}

static napi_value End(napi_env env, napi_callback_info info)
{
    size_t paramNum = ParamNum::TOTAL_ONE;
    napi_value params[ParamNum::TOTAL_ONE] = {0};
    napi_value thisArg = nullptr;
    void* data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &paramNum, params, &thisArg, &data));

    napi_value result = nullptr;
    if (paramNum != ParamNum::TOTAL_ONE) {
        HiLog::Error(LABEL,
            "failed to end trace by trace id, count of parameters is not 1.");
        return result;
    }
    NapiHiTraceNativeCallWrapper::End(env, params, result);
    return result;
}

static napi_value GetId(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    NapiHiTraceNativeCallWrapper::GetId(env, result);
    return result;
}

static napi_value SetId(napi_env env, napi_callback_info info)
{
    size_t paramNum = ParamNum::TOTAL_ONE;
    napi_value params[ParamNum::TOTAL_ONE] = {0};
    napi_value thisArg = nullptr;
    void* data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &paramNum, params, &thisArg, &data));

    napi_value result = nullptr;
    if (paramNum != ParamNum::TOTAL_ONE && paramNum != ParamNum::TOTAL_TWO) {
        HiLog::Error(LABEL,
            "failed to set a new id for a trace, count of parameters is not 1.");
        return result;
    }
    NapiHiTraceNativeCallWrapper::SetId(env, params, result);
    return result;
}

static napi_value ClearId(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    NapiHiTraceNativeCallWrapper::ClearId(env, result);
    return result;
}

static napi_value CreateSpan(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    NapiHiTraceNativeCallWrapper::CreateSpan(env, result);
    return result;
}

static napi_value Tracepoint(napi_env env, napi_callback_info info)
{
    size_t paramNum = ParamNum::TOTAL_FOUR;
    napi_value params[ParamNum::TOTAL_FOUR] = {0};
    napi_value thisArg = nullptr;
    void* data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &paramNum, params, &thisArg, &data));

    napi_value result = nullptr;
    if (paramNum != ParamNum::TOTAL_FOUR) {
        HiLog::Error(LABEL,
            "failed to trace point, count of parameters is not 4.");
        return result;
    }
    NapiHiTraceNativeCallWrapper::Tracepoint(env, params, result);
    return result;
}

static napi_value IsValid(napi_env env, napi_callback_info info)
{
    size_t paramNum = ParamNum::TOTAL_ONE;
    napi_value params[ParamNum::TOTAL_ONE] = {0};
    napi_value thisArg = nullptr;
    void* data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &paramNum, params, &thisArg, &data));

    napi_value result = nullptr;
    if (paramNum != ParamNum::TOTAL_ONE) {
        HiLog::Error(LABEL,
            "failed to check whether a id is valid or not, count of parameters is not 1.");
        return result;
    }
    NapiHiTraceNativeCallWrapper::IsValid(env, params, result);
    return result;
}

static napi_value IsFlagEnabled(napi_env env, napi_callback_info info)
{
    size_t paramNum = ParamNum::TOTAL_TWO;
    napi_value params[ParamNum::TOTAL_TWO] = {0};
    napi_value thisArg = nullptr;
    void* data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &paramNum, params, &thisArg, &data));

    napi_value result = nullptr;
    if (paramNum != ParamNum::TOTAL_TWO) {
        HiLog::Error(LABEL,
            "failed to check whether a flag is enabled in a trace id, count of parameters is not 2.");
        return result;
    }
    NapiHiTraceNativeCallWrapper::IsFlagEnabled(env, params, result);
    return result;
}

static napi_value EnableFlag(napi_env env, napi_callback_info info)
{
    size_t paramNum = ParamNum::TOTAL_TWO;
    napi_value params[ParamNum::TOTAL_TWO] = {0};
    napi_value thisArg = nullptr;
    void* data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &paramNum, params, &thisArg, &data));

    napi_value result = nullptr;
    if (paramNum != ParamNum::TOTAL_TWO) {
        HiLog::Error(LABEL,
            "failed to enable a flag for a trace id, count of parameters is not 2.");
        return result;
    }
    NapiHiTraceNativeCallWrapper::EnableFlag(env, params, result);
    return result;
}

EXTERN_C_START
static napi_value TraceNapiInit(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("begin", Begin),
        DECLARE_NAPI_FUNCTION("end", End),
        DECLARE_NAPI_FUNCTION("getId", GetId),
        DECLARE_NAPI_FUNCTION("setId", SetId),
        DECLARE_NAPI_FUNCTION("clearId", ClearId),
        DECLARE_NAPI_FUNCTION("createSpan", CreateSpan),
        DECLARE_NAPI_FUNCTION("tracepoint", Tracepoint),
        DECLARE_NAPI_FUNCTION("isValid", IsValid),
        DECLARE_NAPI_FUNCTION("isFlagEnabled", IsFlagEnabled),
        DECLARE_NAPI_FUNCTION("enableFlag", EnableFlag),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(napi_property_descriptor), desc));

    // init HiTraceFlag class, HiTraceTracePointType class and HiTraceCommunicationMode class
    InitNapiClass(env, exports);
    return exports;
}
EXTERN_C_END

static napi_module hitrace_module = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = TraceNapiInit,
    .nm_modname = "hiTraceChain",
    .nm_priv = (reinterpret_cast<void *>(0)),
    .reserved = {0}
};

extern "C" __attribute__((constructor)) void RegisterModule(void)
{
    napi_module_register(&hitrace_module);
}
