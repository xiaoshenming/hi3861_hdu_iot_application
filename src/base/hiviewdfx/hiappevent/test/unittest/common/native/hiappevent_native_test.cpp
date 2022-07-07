/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "hiappevent_native_test.h"

#include <ctime>
#include <string>
#include <unistd.h>
#include <vector>

#include "hiappevent/hiappevent.h"
#include "hiappevent_base.h"
#include "hiappevent_config.h"

using namespace testing::ext;
using namespace OHOS::HiviewDFX;

namespace {
const std::string TEST_STORAGE_PATH = "/data/test/hiappevent/";
const std::string DEFAULT_FILE_NAME = "app_event_20210101.log";
const char* TEST_DOMAIN_NAME = "test_domain";
const char* TEST_EVENT_NAME = "test_event";
constexpr int DATE_SIZE = 9;

std::string GetStorageFilePath()
{
    time_t nowTime = time(nullptr);
    if (nowTime == -1) {
        return DEFAULT_FILE_NAME;
    }
    char dateChs[DATE_SIZE] = {0};
    struct tm localTm;
    if (localtime_r(&nowTime, &localTm) == nullptr) {
        return DEFAULT_FILE_NAME;
    }
    if (strftime(dateChs, sizeof(dateChs), "%Y%m%d", &localTm) == 0) {
        return DEFAULT_FILE_NAME;
    }
    std::string dateStr = dateChs;
    return "app_event_" + dateStr + ".log";
}
}

void HiAppEventNativeTest::SetUpTestCase()
{
    HiAppEventConfig::GetInstance().SetStorageDir(TEST_STORAGE_PATH);
}

/**
 * @tc.name: HiAppEventNDKTest001
 * @tc.desc: check the logging function
 * @tc.type: FUNC
 * @tc.require: AR000GIKMA
 */
HWTEST_F(HiAppEventNativeTest, HiAppEventNDKTest001, TestSize.Level0)
{
    /**
     * @tc.steps: step1. create a ParamList pointer.
     * @tc.steps: step2. add params to the ParamList.
     * @tc.steps: step3. write event to the file.
     * @tc.steps: step4. check the result of logging.
     * @tc.steps: step5. destroy the ParamList pointer.
     */
    ParamList list = OH_HiAppEvent_CreateParamList();
    bool boolean = true;
    OH_HiAppEvent_AddBoolParam(list, "bool_key", boolean);
    bool booleans[] = {true, false, true};
    OH_HiAppEvent_AddBoolArrayParam(list, "bool_arr_key", booleans, sizeof(booleans) / sizeof(booleans[0]));
    int8_t num1 = 1;
    OH_HiAppEvent_AddInt8Param(list, "int8_key", num1);
    int8_t nums1[] = {1, INT8_MIN, INT8_MAX};
    OH_HiAppEvent_AddInt8ArrayParam(list, "int8_arr_key", nums1, sizeof(nums1) / sizeof(nums1[0]));
    int16_t num2 = 1;
    OH_HiAppEvent_AddInt16Param(list, "int16_key", num2);
    int16_t nums2[] = {1, INT16_MAX, INT16_MIN};
    OH_HiAppEvent_AddInt16ArrayParam(list, "int16_arr_key", nums2, sizeof(nums2) / sizeof(nums2[0]));
    int32_t num3 = 1;
    OH_HiAppEvent_AddInt32Param(list, "int32_key", num3);
    int32_t nums3[] = {1, INT32_MAX, INT32_MIN};
    OH_HiAppEvent_AddInt32ArrayParam(list, "int32_arr_key", nums3, sizeof(nums3) / sizeof(nums3[0]));
    int64_t num4 = 1;
    OH_HiAppEvent_AddInt64Param(list, "int64_key", num4);
    int64_t nums4[] = {1ll, INT64_MAX, INT64_MIN};
    OH_HiAppEvent_AddInt64ArrayParam(list, "int64_arr_key", nums4, sizeof(nums4) / sizeof(nums4[0]));
    float num5 = 465.1234;
    OH_HiAppEvent_AddFloatParam(list, "float_key", num5);
    float nums5[] = {123.22f, num5, 131312.46464f};
    OH_HiAppEvent_AddFloatArrayParam(list, "float_arr_key", nums5, sizeof(nums5) / sizeof(nums5[0]));
    double num6 = 465.1234;
    OH_HiAppEvent_AddDoubleParam(list, "double_key", num6);
    double nums6[] = {123.22, num6, 131312.46464};
    OH_HiAppEvent_AddDoubleArrayParam(list, "double_arr_key", nums6, sizeof(nums6) / sizeof(nums6[0]));
    char str1[] = "hello";
    OH_HiAppEvent_AddStringParam(list, "str_key", str1);
    char str2[] = "world";
    char* strs[] = {str1, str2};
    OH_HiAppEvent_AddStringArrayParam(list, "str_arr_key", strs, sizeof(strs) / sizeof(strs[0]));

    int res = OH_HiAppEvent_Write(TEST_DOMAIN_NAME, TEST_EVENT_NAME, BEHAVIOR, list);
    OH_HiAppEvent_DestroyParamList(list);
    ASSERT_EQ(res, ErrorCode::HIAPPEVENT_VERIFY_SUCCESSFUL);
}

/**
 * @tc.name: HiAppEventNDKTest002
 * @tc.desc: check the overwriting function of the same param name.
 * @tc.type: FUNC
 * @tc.require: AR000GIKMA
 */
HWTEST_F(HiAppEventNativeTest, HiAppEventNDKTest002, TestSize.Level0)
{
    /**
     * @tc.steps: step1. create a ParamList pointer.
     * @tc.steps: step2. add params with the same name to the ParamList.
     * @tc.steps: step3. write event to the file.
     * @tc.steps: step4. check the result of logging.
     * @tc.steps: step5. destroy the ParamList pointer.
     */
    ParamList list = OH_HiAppEvent_CreateParamList();
    int8_t num1 = 1;
    OH_HiAppEvent_AddInt8Param(list, "int_key", num1);
    int8_t nums1[] = {1, INT8_MIN, INT8_MAX};
    OH_HiAppEvent_AddInt8ArrayParam(list, "int8_arr_key", nums1, sizeof(nums1) / sizeof(nums1[0]));
    int16_t num2 = 1;
    OH_HiAppEvent_AddInt16Param(list, "int16_key", num2);
    int16_t nums2[] = {1, INT16_MAX, INT16_MIN};
    OH_HiAppEvent_AddInt16ArrayParam(list, "int16_key", nums2, sizeof(nums2) / sizeof(nums2[0]));

    int res = OH_HiAppEvent_Write(TEST_DOMAIN_NAME, TEST_EVENT_NAME, BEHAVIOR, list);
    OH_HiAppEvent_DestroyParamList(list);
    ASSERT_EQ(res, ErrorCode::HIAPPEVENT_VERIFY_SUCCESSFUL);
}

/**
 * @tc.name: HiAppEventNDKTest003
 * @tc.desc: check the logging function when the input value is nullptr.
 * @tc.type: FUNC
 * @tc.require: AR000GIKMA
 */
HWTEST_F(HiAppEventNativeTest, HiAppEventNDKTest003, TestSize.Level0)
{
    /**
     * @tc.steps: step1. create a ParamList pointer.
     * @tc.steps: step2. add params with the nullptr value to the ParamList.
     * @tc.steps: step3. write event to the file.
     * @tc.steps: step4. check the result of logging.
     * @tc.steps: step5. destroy the ParamList pointer.
     */
    int res = OH_HiAppEvent_Write(TEST_DOMAIN_NAME, TEST_EVENT_NAME, FAULT, nullptr);
    ASSERT_EQ(res, ErrorCode::HIAPPEVENT_VERIFY_SUCCESSFUL);

    ParamList list = OH_HiAppEvent_CreateParamList();
    bool boolean = true;
    OH_HiAppEvent_AddBoolParam(list, nullptr, boolean);
    OH_HiAppEvent_AddBoolArrayParam(list, "bool_arr_key", nullptr, 0);
    int8_t num1 = 1;
    OH_HiAppEvent_AddInt8Param(list, nullptr, num1);
    OH_HiAppEvent_AddInt8ArrayParam(list, "int8_arr_key", nullptr, 0);
    int16_t num2 = 1;
    OH_HiAppEvent_AddInt16Param(list, nullptr, num2);
    OH_HiAppEvent_AddInt16ArrayParam(list, "int16_arr_key", nullptr, 1);
    int32_t num3 = 1;
    OH_HiAppEvent_AddInt32Param(list, nullptr, num3);
    OH_HiAppEvent_AddInt32ArrayParam(list, "int32_arr_key", nullptr, 2);
    int64_t num4 = 1;
    OH_HiAppEvent_AddInt64Param(list, nullptr, num4);
    OH_HiAppEvent_AddInt64ArrayParam(list, "int64_arr_key", nullptr, 3);
    float num5 = 465.1234;
    OH_HiAppEvent_AddFloatParam(list, nullptr, num5);
    OH_HiAppEvent_AddFloatArrayParam(list, "float_arr_key", nullptr, -1);
    double num6 = 465.1234;
    OH_HiAppEvent_AddDoubleParam(list, nullptr, num6);
    OH_HiAppEvent_AddDoubleArrayParam(list, "double_arr_key", nullptr, 0);
    char str1[] = "hello";
    OH_HiAppEvent_AddStringParam(list, nullptr, str1);
    OH_HiAppEvent_AddStringParam(list, nullptr, nullptr);
    OH_HiAppEvent_AddStringArrayParam(list, "str_arr_key", nullptr, 0);
    char* strs[] = {str1, nullptr};
    OH_HiAppEvent_AddStringArrayParam(list, "str_arr_null_key", strs, sizeof(strs) / sizeof(strs[0]));

    res = OH_HiAppEvent_Write(TEST_DOMAIN_NAME, TEST_EVENT_NAME, STATISTIC, list);
    OH_HiAppEvent_DestroyParamList(list);
    ASSERT_EQ(res, ErrorCode::HIAPPEVENT_VERIFY_SUCCESSFUL);
}

/**
 * @tc.name: HiAppEventNDKTest004
 * @tc.desc: check the verification function of event logging.
 * @tc.type: FUNC
 * @tc.require: AR000GIKMA
 */
HWTEST_F(HiAppEventNativeTest, HiAppEventNDKTest004, TestSize.Level0)
{
    /**
     * @tc.steps: step1. create a ParamList pointer.
     * @tc.steps: step2. add params with the invalid name to the ParamList.
     * @tc.steps: step3. write event to the file.
     * @tc.steps: step4. check the result of logging.
     * @tc.steps: step5. destroy the ParamList pointer.
     */
    ParamList list = OH_HiAppEvent_CreateParamList();
    char key1[] = "**";
    int8_t num1 = 1;
    OH_HiAppEvent_AddInt8Param(list, key1, num1);
    char key2[] = "HH22";
    int16_t num2 = 1;
    OH_HiAppEvent_AddInt16Param(list, key2, num2);
    char key3[] = "aa_";
    int32_t num3 = 1;
    OH_HiAppEvent_AddInt32Param(list, key3, num3);
    char key4[] = "";
    int64_t num4 = 1;
    OH_HiAppEvent_AddInt64Param(list, key4, num4);

    int res = OH_HiAppEvent_Write(TEST_DOMAIN_NAME, TEST_EVENT_NAME, SECURITY, list);
    OH_HiAppEvent_DestroyParamList(list);
    ASSERT_EQ(res,  ErrorCode::ERROR_INVALID_PARAM_NAME);
}

/**
 * @tc.name: HiAppEventNDKTest005
 * @tc.desc: check the verification function of event logging.
 * @tc.type: FUNC
 * @tc.require: AR000GIKMA
 */
HWTEST_F(HiAppEventNativeTest, HiAppEventNDKTest005, TestSize.Level0)
{
    /**
     * @tc.steps: step1. create a ParamList pointer.
     * @tc.steps: step2. add params with too long string length to the ParamList.
     * @tc.steps: step3. write event to the file.
     * @tc.steps: step4. check the result of logging.
     * @tc.steps: step5. destroy the ParamList pointer.
     */
    int maxStrLen = 8 * 1024;
    std::string longStr(maxStrLen, 'a');
    std::string longInvalidStr(maxStrLen + 1, 'a');
    const char* strs[] = {"hello", longStr.c_str()};
    const char* strIns[] = {"hello", longInvalidStr.c_str()};

    ParamList list = OH_HiAppEvent_CreateParamList();
    OH_HiAppEvent_AddStringParam(list, "long_s_key", longStr.c_str());
    OH_HiAppEvent_AddStringArrayParam(list, "long_s_a_key", strs, sizeof(strs) / sizeof(strs[0]));
    OH_HiAppEvent_AddStringParam(list, "long_s_i_key", longInvalidStr.c_str());
    OH_HiAppEvent_AddStringArrayParam(list, "long_s_a_i_key", strIns, sizeof(strIns) / sizeof(strIns[0]));

    int res = OH_HiAppEvent_Write(TEST_DOMAIN_NAME, TEST_EVENT_NAME, SECURITY, list);
    OH_HiAppEvent_DestroyParamList(list);
    ASSERT_EQ(res,  ErrorCode::ERROR_INVALID_PARAM_VALUE_LENGTH);
}

/**
 * @tc.name: HiAppEventNDKTest006
 * @tc.desc: check the verification function of event logging.
 * @tc.type: FUNC
 * @tc.require: AR000GIKMA
 */
HWTEST_F(HiAppEventNativeTest, HiAppEventNDKTest006, TestSize.Level0)
{
    /**
     * @tc.steps: step1. create a ParamList pointer.
     * @tc.steps: step2. add too many params to the ParamList.
     * @tc.steps: step3. write event to the file.
     * @tc.steps: step4. check the result of logging.
     * @tc.steps: step5. destroy the ParamList pointer.
     */
    // max len is 32
    int len = 33;
    std::vector<std::string> keys(len);
    std::vector<std::string> values(len);
    ParamList list = OH_HiAppEvent_CreateParamList();
    for (int i = 0; i < len; i++) {
        keys[i] = "key" + std::to_string(i);
        values[i] = "value" + std::to_string(i);
        OH_HiAppEvent_AddStringParam(list, keys[i].c_str(), values[i].c_str());
    }

    int res = OH_HiAppEvent_Write(TEST_DOMAIN_NAME, TEST_EVENT_NAME, SECURITY, list);
    OH_HiAppEvent_DestroyParamList(list);
    ASSERT_EQ(res,  ErrorCode::ERROR_INVALID_PARAM_NUM);
}

/**
 * @tc.name: HiAppEventNDKTest007
 * @tc.desc: check the verification function of event logging.
 * @tc.type: FUNC
 * @tc.require: AR000GIKMA
 */
HWTEST_F(HiAppEventNativeTest, HiAppEventNDKTest007, TestSize.Level0)
{
    /**
     * @tc.steps: step1. create a ParamList pointer.
     * @tc.steps: step2. add params to the ParamList.
     * @tc.steps: step3. write event with invalid event name to the file.
     * @tc.steps: step4. check the result of logging.
     * @tc.steps: step5. destroy the ParamList pointer.
     */
    ParamList list = OH_HiAppEvent_CreateParamList();
    OH_HiAppEvent_AddInt32Param(list, "int_key", 1);
    int res = OH_HiAppEvent_Write(TEST_DOMAIN_NAME, "verify_**", SECURITY, list);
    OH_HiAppEvent_DestroyParamList(list);
    ASSERT_EQ(res,  ErrorCode::ERROR_INVALID_EVENT_NAME);

    list = OH_HiAppEvent_CreateParamList();
    OH_HiAppEvent_AddInt32Param(list, "int_key", 2);
    res = OH_HiAppEvent_Write(TEST_DOMAIN_NAME, "verify_TEST", SECURITY, list);
    OH_HiAppEvent_DestroyParamList(list);
    ASSERT_EQ(res,  ErrorCode::ERROR_INVALID_EVENT_NAME);

    list = OH_HiAppEvent_CreateParamList();
    OH_HiAppEvent_AddInt32Param(list, "int_key", 3);
    res = OH_HiAppEvent_Write(TEST_DOMAIN_NAME, "", SECURITY, list);
    OH_HiAppEvent_DestroyParamList(list);
    ASSERT_EQ(res,  ErrorCode::ERROR_INVALID_EVENT_NAME);
}

/**
 * @tc.name: HiAppEventNDKTest008
 * @tc.desc: check the verification function of event logging.
 * @tc.type: FUNC
 * @tc.require: AR000GIKMA
 */
HWTEST_F(HiAppEventNativeTest, HiAppEventNDKTest008, TestSize.Level0)
{
    /**
     * @tc.steps: step1. create a ParamList pointer.
     * @tc.steps: step2. add params to the ParamList.
     * @tc.steps: step3. write event with nullptr event name to the file.
     * @tc.steps: step4. check the result of logging.
     * @tc.steps: step5. destroy the ParamList pointer.
     */
    ParamList list = OH_HiAppEvent_CreateParamList();
    OH_HiAppEvent_AddInt32Param(list, "int_key", 1);

    int res = OH_HiAppEvent_Write(TEST_DOMAIN_NAME, nullptr, SECURITY, list);
    OH_HiAppEvent_DestroyParamList(list);
    ASSERT_EQ(res,  ErrorCode::ERROR_INVALID_EVENT_NAME);
}

/**
 * @tc.name: HiAppEventNDKTest009
 * @tc.desc: check the verification function of event logging.
 * @tc.type: FUNC
 * @tc.require: AR000GIKMA
 */
HWTEST_F(HiAppEventNativeTest, HiAppEventNDKTest009, TestSize.Level0)
{
    /**
     * @tc.steps: step1. disable the logging function.
     * @tc.steps: step2. create a ParamList pointer.
     * @tc.steps: step3. add params to the ParamList.
     * @tc.steps: step4. write event to the file.
     * @tc.steps: step5. check the result of logging.
     * @tc.steps: step6. destroy the ParamList pointer.
     */
    OH_HiAppEvent_Configure(DISABLE, "true");
    ParamList list = OH_HiAppEvent_CreateParamList();
    OH_HiAppEvent_AddInt32Param(list, "int_key", 1);
    int res = OH_HiAppEvent_Write(TEST_DOMAIN_NAME, TEST_EVENT_NAME, SECURITY, list);
    OH_HiAppEvent_DestroyParamList(list);
    ASSERT_EQ(res,  ErrorCode::ERROR_HIAPPEVENT_DISABLE);

    OH_HiAppEvent_Configure(DISABLE, "false");
    list = OH_HiAppEvent_CreateParamList();
    OH_HiAppEvent_AddStringParam(list, "str_key", "test");
    res = OH_HiAppEvent_Write(TEST_DOMAIN_NAME, TEST_EVENT_NAME, SECURITY, list);
    OH_HiAppEvent_DestroyParamList(list);
    ASSERT_EQ(res,  ErrorCode::HIAPPEVENT_VERIFY_SUCCESSFUL);
}

/**
 * @tc.name: HiAppEventNDKTest010
 * @tc.desc: check the configuration function of event logging.
 * @tc.type: FUNC
 * @tc.require: AR000GIKMA
 */
HWTEST_F(HiAppEventNativeTest, HiAppEventNDKTest010, TestSize.Level0)
{
    /**
     * @tc.steps: step1. config with invalid params.
     * @tc.steps: step2. check the result of config.
     */
    bool res = OH_HiAppEvent_Configure(nullptr, nullptr);
    ASSERT_FALSE(res);

    res = OH_HiAppEvent_Configure("key", "true");
    ASSERT_FALSE(res);

    res = OH_HiAppEvent_Configure(DISABLE, "xxx");
    ASSERT_FALSE(res);

    res = OH_HiAppEvent_Configure(MAX_STORAGE, "xxx");
    ASSERT_FALSE(res);

    res = OH_HiAppEvent_Configure("", "100M");
    ASSERT_FALSE(res);
}

/**
 * @tc.name: HiAppEventNDKTest011
 * @tc.desc: check the configuration function of event logging.
 * @tc.type: FUNC
 * @tc.require: AR000GIKMA
 */
HWTEST_F(HiAppEventNativeTest, HiAppEventNDKTest011, TestSize.Level0)
{
    /**
     * @tc.steps: step1. config the storage directory quota of the logging function.
     * @tc.steps: step2. check the result of config.
     * @tc.steps: step3. write event to the file.
     * @tc.steps: step4. check the result of logging.
     */
    bool res = OH_HiAppEvent_Configure(MAX_STORAGE, "1k");
    ASSERT_TRUE(res);

    ParamList list = OH_HiAppEvent_CreateParamList();
    OH_HiAppEvent_AddInt32Param(list, "int_key", 1);
    int result = OH_HiAppEvent_Write(TEST_DOMAIN_NAME, TEST_EVENT_NAME, SECURITY, list);
    OH_HiAppEvent_DestroyParamList(list);
    ASSERT_EQ(result,  ErrorCode::HIAPPEVENT_VERIFY_SUCCESSFUL);

    res = OH_HiAppEvent_Configure(MAX_STORAGE, "100M");
    ASSERT_TRUE(res);
}

/**
 * @tc.name: HiAppEventNDKTest012
 * @tc.desc: check the event logging function with predefined events.
 * @tc.type: FUNC
 * @tc.require: AR000GIKMA
 */
HWTEST_F(HiAppEventNativeTest, HiAppEventNDKTest012, TestSize.Level0)
{
    /**
     * @tc.steps: step1. create a ParamList pointer.
     * @tc.steps: step2. add params with predefined param name to the ParamList.
     * @tc.steps: step3. write event with predefined event name to the file.
     * @tc.steps: step4. check the result of logging.
     * @tc.steps: step5. destroy the ParamList pointer.
     */
    ParamList list = OH_HiAppEvent_CreateParamList();
    OH_HiAppEvent_AddInt32Param(list, PARAM_USER_ID, 123);
    int res = OH_HiAppEvent_Write(TEST_DOMAIN_NAME, EVENT_USER_LOGIN, BEHAVIOR, list);
    OH_HiAppEvent_DestroyParamList(list);
    ASSERT_EQ(res,  ErrorCode::HIAPPEVENT_VERIFY_SUCCESSFUL);

    list = OH_HiAppEvent_CreateParamList();
    OH_HiAppEvent_AddStringParam(list, PARAM_USER_ID, "123456");
    res = OH_HiAppEvent_Write(TEST_DOMAIN_NAME, EVENT_USER_LOGOUT, SECURITY, list);
    OH_HiAppEvent_DestroyParamList(list);
    ASSERT_EQ(res,  ErrorCode::HIAPPEVENT_VERIFY_SUCCESSFUL);

    list = OH_HiAppEvent_CreateParamList();
    OH_HiAppEvent_AddStringParam(list, PARAM_DISTRIBUTED_SERVICE_NAME, "hiview");
    OH_HiAppEvent_AddStringParam(list, PARAM_DISTRIBUTED_SERVICE_INSTANCE_ID, "123");
    res = OH_HiAppEvent_Write(TEST_DOMAIN_NAME, EVENT_DISTRIBUTED_SERVICE_START, SECURITY, list);
    OH_HiAppEvent_DestroyParamList(list);
    ASSERT_EQ(res,  ErrorCode::HIAPPEVENT_VERIFY_SUCCESSFUL);
}

/**
 * @tc.name: HiAppEventNDKTest013
 * @tc.desc: check the local file.
 * @tc.type: FUNC
 * @tc.require: AR000GIKMA
 */
HWTEST_F(HiAppEventNativeTest, HiAppEventNDKTest013, TestSize.Level0)
{
    /**
     * @tc.steps: step1. create a ParamList pointer.
     * @tc.steps: step2. add params to the ParamList.
     * @tc.steps: step3. write event to the file.
     * @tc.steps: step4. check the result of logging.
     * @tc.steps: step5. check the file.
     */
    ParamList list = OH_HiAppEvent_CreateParamList();
    OH_HiAppEvent_AddInt32Param(list, "int_key", 123);
    int res = OH_HiAppEvent_Write(TEST_DOMAIN_NAME, TEST_EVENT_NAME, BEHAVIOR, list);
    OH_HiAppEvent_DestroyParamList(list);
    ASSERT_EQ(res,  ErrorCode::HIAPPEVENT_VERIFY_SUCCESSFUL);

    std::string filePath = TEST_STORAGE_PATH + GetStorageFilePath();
    ASSERT_EQ(access(filePath.c_str(), F_OK), 0);
}
