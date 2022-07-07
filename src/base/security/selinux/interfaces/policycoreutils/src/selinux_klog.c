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

#include "selinux_klog.h"
#include <fcntl.h>
#include <unistd.h>
#include "securec.h"

static int g_logLevel = SELINUX_KERROR;
static const char *LOG_LEVEL_STR[] = {"ERROR", "WARNING", "INFO", "AVC"};

#ifndef UNLIKELY
#define UNLIKELY(x) __builtin_expect(!!(x), 0)
#endif

void SetSelinuKLogLevel(int logLevel)
{
    g_logLevel = logLevel;
}

static int g_fd = -1;
static void SelinuxOpenLogDevice(void)
{
    int fd = open("/dev/kmsg", O_WRONLY | O_CLOEXEC, S_IRUSR | S_IWUSR | S_IRGRP | S_IRGRP);
    if (fd >= 0) {
        g_fd = fd;
    }
    return;
}

int SelinuKLog(int logLevel, const char *fmt, ...)
{
    if (logLevel != SELINUX_KAVC && logLevel > g_logLevel) {
        return -1;
    }

    if (UNLIKELY(g_fd < 0)) {
        SelinuxOpenLogDevice();
        if (g_fd < 0) {
            return -1;
        }
    }
    va_list vargs;
    va_start(vargs, fmt);
    char tmpFmt[MAX_LOG_SIZE];
    if (vsnprintf_s(tmpFmt, MAX_LOG_SIZE, MAX_LOG_SIZE - 1, fmt, vargs) == -1) {
        close(g_fd);
        g_fd = -1;
        va_end(vargs);
        return -1;
    }

    char logInfo[MAX_LOG_SIZE];
    int res = 0;
    if (logLevel != SELINUX_KAVC) {
        res = snprintf_s(logInfo, MAX_LOG_SIZE, MAX_LOG_SIZE - 1, "[pid=%d][%s][%s] %s", getpid(), "SELINUX",
                         LOG_LEVEL_STR[logLevel], tmpFmt);
    } else {
        res = snprintf_s(logInfo, MAX_LOG_SIZE, MAX_LOG_SIZE - 1, "%s", tmpFmt);
    }
    if (res == -1) {
        close(g_fd);
        g_fd = -1;
        va_end(vargs);
        return -1;
    }
    va_end(vargs);

    if (write(g_fd, logInfo, strlen(logInfo)) < 0) {
        close(g_fd);
        g_fd = -1;
    }
    return 0;
}
