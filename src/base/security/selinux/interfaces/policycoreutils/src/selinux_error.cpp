/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "selinux_error.h"
#include <cmath>

namespace Selinux {
static const char *g_selinuxErrMsg[SELINUX_ERROR_MAX] = {
    [SELINUX_SUCC] = "success",
    [SELINUX_ARG_INVALID] = "argument is invalid",
    [SELINUX_PATH_INVAILD] = "path is invalid",
    [SELINUX_STAT_INVAILD] = "stat failed",
    [SELINUX_PTR_NULL] = "ptr is null",
    [SELINUX_KEY_NOT_FOUND] = "cannot find key in contexts file",
    [SELINUX_GET_CONTEXT_ERROR] = "get context failed",
    [SELINUX_SET_CONTEXT_ERROR] = "set context failed",
    [SELINUX_SET_CONTEXT_TYPE_ERROR] = "set context type failed",
    [SELINUX_CHECK_CONTEXT_ERROR] = "check context failed, context may not define",
    [SELINUX_CONTEXTS_FILE_LOAD_ERROR] = "load contexts file failed",
    [SELINUX_FTS_OPEN_ERROR] = "fts_open failed",
    [SELINUX_FTS_ELOOP] = "fts ELOOP",
    [SELINUX_RESTORECON_ERROR] = "hap path restorecon error",
    [SELINUX_UNKNOWN_ERROR] = "unknown error",
    [SELINUX_PERMISSION_DENY] = "permission denied",
};

const char *GetErrStr(int errNo)
{
    int errIndex = std::abs(errNo);
    errIndex = errIndex >= SELINUX_ERROR_MAX ? SELINUX_UNKNOWN_ERROR : errIndex;
    return g_selinuxErrMsg[errIndex];
}
} // namespace Selinux
