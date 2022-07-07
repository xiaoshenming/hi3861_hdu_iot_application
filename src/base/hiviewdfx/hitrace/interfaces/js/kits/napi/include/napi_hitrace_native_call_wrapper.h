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

#ifndef HIVIEWDFX_NAPI_HITRACE_NATIVE_CALL_WRAPPER_H
#define HIVIEWDFX_NAPI_HITRACE_NATIVE_CALL_WRAPPER_H

#include "hitrace/trace.h"

#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace HiviewDFX {
class NapiHiTraceNativeCallWrapper {
public:
    static void Begin(napi_env env, size_t paramNum, napi_value* params, napi_value& result);
    static void End(napi_env env, napi_value* params, napi_value& result);
    static void GetId(napi_env env, napi_value& result);
    static void SetId(napi_env env, napi_value* params, napi_value& result);
    static void ClearId(napi_env env, napi_value& result);
    static void CreateSpan(napi_env env, napi_value& result);
    static void Tracepoint(napi_env env, napi_value* params, napi_value& result);
    static void IsValid(napi_env env, napi_value* params, napi_value& result);
    static void IsFlagEnabled(napi_env env, napi_value* params, napi_value& result);
    static void EnableFlag(napi_env env, napi_value* params, napi_value& result);
};
} // namespace HiviewDFX
} // namespace OHOS

#endif // HIVIEWDFX_NAPI_HITRACE_NATIVE_CALL_WRAPPER_H
