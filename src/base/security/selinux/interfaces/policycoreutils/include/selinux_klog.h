/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef SELINUX_KLOG_H
#define SELINUX_KLOG_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define MAX_LOG_SIZE 1024

typedef enum SelinuxKLogLevel {
    SELINUX_KERROR = 0,
    SELINUX_KWARN,
    SELINUX_KINFO,
    SELINUX_KAVC
} SelinuxKLogLevel;

void SetSelinuKLogLevel(int logLevel);
int SelinuKLog(int logLevel, const char *fmt, ...);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif // SELINUX_KLOG_H
