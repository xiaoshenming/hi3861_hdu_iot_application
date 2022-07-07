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

#ifndef SELINUX_ERROE_H
#define SELINUX_ERROE_H

namespace Selinux {
enum Errno {
    SELINUX_SUCC,
    SELINUX_ARG_INVALID,
    SELINUX_PATH_INVAILD,
    SELINUX_STAT_INVAILD,
    SELINUX_PTR_NULL,
    SELINUX_KEY_NOT_FOUND,
    SELINUX_GET_CONTEXT_ERROR,
    SELINUX_SET_CONTEXT_ERROR,
    SELINUX_SET_CONTEXT_TYPE_ERROR,
    SELINUX_CHECK_CONTEXT_ERROR,
    SELINUX_CONTEXTS_FILE_LOAD_ERROR,
    SELINUX_FTS_OPEN_ERROR,
    SELINUX_FTS_ELOOP,
    SELINUX_RESTORECON_ERROR,
    SELINUX_UNKNOWN_ERROR,
    SELINUX_PERMISSION_DENY,
    SELINUX_ERROR_MAX,
};

const char *GetErrStr(int errNo);
} // namespace Selinux

#endif // SELINUX_ERROE_H
