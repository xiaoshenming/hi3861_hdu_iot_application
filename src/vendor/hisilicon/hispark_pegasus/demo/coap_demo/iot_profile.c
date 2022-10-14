/*
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies CO., LIMITED.
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

#include <cJSON.h>
#include <string.h>
#include <hi_mem.h>
#include <hi_stdlib.h>
#include "iot_config.h"
#include "iot_log.h"
#include "iot_errno.h"
#include "iot_main.h"
#include "hi_types_base.h"
#include "iot_profile.h"

#define CN_PROFILE_SERVICE_KEY_SERVICEID   "service_id"
#define CN_PROFILE_SERVICE_KEY_PROPERTIIES "properties"
#define CN_PROFILE_SERVICE_KEY_EVENTTIME   "event_time"
#define CN_PROFILE_KEY_SERVICES            "services"
// < use this function to make a topic to publish
// < if request_id  is needed depends on the fmt
static char* MakeTopic(const char* fmt, const char* deviceID, const char* requestID)
{
    int len;
    char* ret = NULL;

    len = strlen(fmt) + strlen(deviceID);
    if (requestID != NULL) {
        len += strlen(requestID);
    }

    ret = hi_malloc(0, len);
    if (ret != NULL) {
        if (requestID != NULL) {
            (void)snprintf_s(ret, len, len, fmt, deviceID, requestID);
        } else {
            (void)snprintf_s(ret, len, len, fmt, deviceID);
        }
    }
    return ret;
}

#define CN_PROFILE_CMDRESP_KEY_RETCODE  "result_code"
#define CN_PROFILE_CMDRESP_KEY_RESPNAME "response_name"
#define CN_PROFILE_CMDRESP_KEY_PARAS    "paras"

#define WECHAT_SUBSCRIBE_TYPE    "update"
#define WECHAT_SUBSCRIBE_VERSION "1.0.0"
#define WECHAT_SUBSCRIBE_TOKEN   "DeviceSubscribe"
static char* MakeProfileReport(WeChatProfile* payload)
{
    char* ret = NULL;
    cJSON* root;
    cJSON* state;
    cJSON* reported;

    /* create json root node */
    root = cJSON_CreateObject();
    if (root == NULL) {
        return ret;
    }
    /* state create */
    state = cJSON_CreateObject();
    if (state == NULL) {
        return ret;
    }
    /* reported create */
    reported = cJSON_CreateObject();
    if (reported == NULL) {
        return ret;
    }
    /* add root object */
    cJSON_AddItemToObject(root, payload->subscribeType, cJSON_CreateString(WECHAT_SUBSCRIBE_TYPE));
    cJSON_AddItemToObject(root, payload->status.subState, state);
    cJSON_AddItemToObject(state, payload->status.subReport, reported);
    cJSON_AddItemToObject(root, payload->status.reportVersion, cJSON_CreateString(WECHAT_SUBSCRIBE_VERSION));
    cJSON_AddItemToObject(root, payload->status.Token, cJSON_CreateString(WECHAT_SUBSCRIBE_TOKEN));
    /* add reported item */
    cJSON_AddNumberToObject(reported, payload->reportAction.subDeviceActionLight,
                            payload->reportAction.lightActionStatus);
    cJSON_AddNumberToObject(reported, payload->reportAction.subDeviceActionMotor,
                            payload->reportAction.motorActionStatus);
    cJSON_AddNumberToObject(reported, payload->reportAction.subDeviceActionTemperature,
                            payload->reportAction.temperatureData);
    cJSON_AddNumberToObject(reported, payload->reportAction.subDeviceActionHumidity,
                            payload->reportAction.humidityActionData);
    cJSON_AddNumberToObject(reported, payload->reportAction.subDeviceActionLightIntensity,
                            payload->reportAction.lightIntensityActionData);
    ret = cJSON_PrintUnformatted(root);
    cJSON_Delete(state);
    cJSON_Delete(reported);
    cJSON_Delete(root);
    return ret;
}

#define CN_PROFILE_TOPICFMT_TOPIC "$shadow/operation/19VUBHD786/mqtt"
int IoTProfilePropertyReport(char* deviceID, WeChatProfile* payload)
{
    int ret = -1;
    char* topic;
    char* msg;

    if ((deviceID == NULL) || (payload == NULL)) {
        return ret;
    }
    topic = MakeTopic(CN_PROFILE_TOPICFMT_TOPIC, deviceID, NULL);
    if (topic == NULL) {
        return ret;
    }
    msg = MakeProfileReport(payload);
    if ((topic != NULL) && (msg != NULL)) {
        ret = IotSendMsg(0, topic, msg);
    }

    hi_free(0, topic);
    cJSON_free(msg);

    return ret;
}