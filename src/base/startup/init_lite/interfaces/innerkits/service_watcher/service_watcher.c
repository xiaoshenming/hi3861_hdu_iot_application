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
#include "service_watcher.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "beget_ext.h"
#include "init_utils.h"
#include "securec.h"
#include "service_control.h"

static void ServiceStateChange(const char *key, const char *value, void *context)
{
    ServiceStatusChangePtr callback = (ServiceStatusChangePtr)context;
    int size = 0;
    const InitArgInfo *statusMap = GetServieStatusMap(&size);
    ServiceStatus status = (ServiceStatus)GetMapValue(value, statusMap, size, SERVICE_IDLE);
    if (strlen(key) > strlen(STARTUP_SERVICE_CTL)) {
        callback(key + strlen(STARTUP_SERVICE_CTL) + 1, status);
    } else {
        BEGET_LOGE("Invalid service name %s %s", key, value);
    }
}

int ServiceWatchForStatus(const char *serviceName, ServiceStatusChangePtr changeCallback)
{
    if (serviceName == NULL) {
        BEGET_LOGE("Service wait failed, service is null.");
        return -1;
    }
    char paramName[PARAM_NAME_LEN_MAX] = {0};
    if (snprintf_s(paramName, PARAM_NAME_LEN_MAX, PARAM_NAME_LEN_MAX - 1,
        "%s.%s", STARTUP_SERVICE_CTL, serviceName) == -1) {
        BEGET_LOGE("Failed snprintf_s err=%d", errno);
        return -1;
    }
    if (SystemWatchParameter(paramName, ServiceStateChange, (void *)changeCallback) != 0) {
        BEGET_LOGE("Wait param for %s failed.", paramName);
        return -1;
    }
    return 0;
}
