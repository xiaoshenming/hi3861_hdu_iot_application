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

#include "selinux_unit_test.h"
#include <fstream>
#include <thread>
#include <dirent.h>
#include <selinux/selinux.h>
#include "selinux_error.h"
#include "selinux_parameter.h"
#include "service_checker.h"
#include "hdf_service_checker.h"

using namespace testing::ext;
using namespace OHOS::Security::Selinux;
using namespace Selinux;
const static int SLEEP_SECOND = 2;
const static std::string BASE_PATH = "/data/app/el1/0/base/";
const static std::string TEST_PATH = BASE_PATH + "com.ohos.selftest/";

const static std::string TEST_SUB_PATH_1 = TEST_PATH + "subpath1/";
const static std::string TEST_SUB_PATH_2 = TEST_PATH + "subpath2/";
const static std::string TEST_SUB_PATH_3 = TEST_PATH + "subpath3/";
const static std::string TEST_SUB_PATH_4 = TEST_PATH + "subpath4/";

const static std::string TEST_SUB_PATH_1_FILE_1 = TEST_SUB_PATH_1 + "file1.txt";
const static std::string TEST_SUB_PATH_1_FILE_2 = TEST_SUB_PATH_1 + "file2.txt";
const static std::string TEST_SUB_PATH_2_FILE_1 = TEST_SUB_PATH_2 + "file1.txt";
const static std::string TEST_SUB_PATH_2_FILE_2 = TEST_SUB_PATH_2 + "file2.txt";
const static std::string TEST_SUB_PATH_3_FILE_1 = TEST_SUB_PATH_3 + "file1.txt";
const static std::string TEST_SUB_PATH_3_FILE_2 = TEST_SUB_PATH_3 + "file2.txt";
const static std::string TEST_SUB_PATH_4_FILE_1 = TEST_SUB_PATH_4 + "file1.txt";
const static std::string TEST_SUB_PATH_4_FILE_2 = TEST_SUB_PATH_4 + "file2.txt";

const static std::string TEST_UNSIMPLIFY_PATH = TEST_SUB_PATH_3 + "//../subpath4/";
const static std::string TEST_UNSIMPLIFY_FILE = TEST_SUB_PATH_4 + "//../subpath3/file1.txt";

const static std::string INVALID_PATH = "/data/data/path";
const static std::string NOT_EXIST_PATH = BASE_PATH + "not_exsit_path";
const static std::string TEST_APL = "system_core";
const static std::string TEST_NAME = "com.hap.selftest";
const static std::string DEST_LABEL = "u:object_r:selftest_hap_data_file:s0";
const static std::string DEST_DOMAIN = "u:r:selftest:s0";

const static std::string SEHAP_CONTEXTS_FILE = "/system/etc/selinux/targeted/contexts/sehap_contexts";
const static std::string PARAM_CONTEXTS_FILE = "/system/etc/selinux/targeted/contexts/parameter_contexts";

const static std::string TEST_PARA_NAME = "test.para";
const static std::string TEST_NOT_EXIST_PARA_NAME = "test.not.exist";
const static std::string TEST_PARA_CONTEXT = "u:object_r:testpara:s0";
const static std::string DEFAULT_PARA_CONTEXT = "u:object_r:default_param:s0";

const static std::vector<std::string> TEST_INVALID_PARA = {{".test"}, {"test."}, {"test..test"}, {""}, {"test+test"}};

const static std::string TEST_SERVICE_NAME = "test_service";
const static std::string DEFAULT_SERVICE = "default_service";
const static std::string DEFAULT_HDF_SERVICE = "default_hdf_service";

static bool CreateDirectory(const std::string &path)
{
    std::string::size_type index = 0;
    do {
        std::string subPath;
        index = path.find('/', index + 1);
        if (index == std::string::npos) {
            subPath = path;
        } else {
            subPath = path.substr(0, index);
        }

        if (access(subPath.c_str(), F_OK) != 0) {
            if (mkdir(subPath.c_str(), S_IRWXU) != 0) {
                return false;
            }
        }
    } while (index != std::string::npos);

    return access(path.c_str(), F_OK) == 0;
}

static bool RemoveDirectory(const std::string &path)
{
    const char *dir = path.c_str();
    std::string curDir = ".";
    std::string upDir = "..";
    DIR *dirp;
    struct dirent *dp;
    struct stat dirStat;

    if (access(dir, F_OK) != 0) {
        return true;
    }
    int statRet = stat(dir, &dirStat);
    if (statRet < 0) {
        return false;
    }

    if (S_ISREG(dirStat.st_mode)) {
        remove(dir);
    } else if (S_ISDIR(dirStat.st_mode)) {
        dirp = opendir(dir);
        while ((dp = readdir(dirp)) != nullptr) {
            if ((curDir == std::string(dp->d_name)) || (upDir == std::string(dp->d_name))) {
                continue;
            }
            std::string dirName = std::string(dir) + "/" + std::string(dp->d_name);
            RemoveDirectory(dirName.c_str());
        }
        closedir(dirp);
        rmdir(dir);
    } else {
        return false;
    }
    return true;
}

static std::string GetDirectory(const std::string &path)
{
    std::string dir = "";
    size_t index = path.rfind('/');
    if (std::string::npos != index) {
        dir = path.substr(0, index);
    }
    return dir;
}

static bool CreateFile(const std::string &path)
{
    std::string dir = GetDirectory(path);
    if (dir != "") {
        if (!CreateDirectory(dir)) {
            return false;
        }
    }

    if (access(path.c_str(), F_OK) != 0) {
        FILE *fp = fopen(path.c_str(), "w");
        if (fp == nullptr) {
            return false;
        }
        fclose(fp);
    }

    return access(path.c_str(), F_OK) == 0;
}

static bool CopyFile(const std::string &src, const std::string &des)
{
    std::ifstream fin(src, std::ifstream::in || std::ifstream::binary);
    if (!fin) {
        return false;
    }
    std::ofstream fout(des, std::ofstream::out || std::ofstream::binary);
    if (!fout) {
        fin.close();
        return false;
    }
    fout << fin.rdbuf();
    if (!fout) {
        fin.close();
        fout.close();
        return false;
    }
    fin.close();
    fout.close();
    return true;
}

static bool WriteFile(const std::string &file, const std::vector<std::string> &info)
{
    std::ofstream fout(file, std::ofstream::out || std::ofstream::app);
    if (!fout) {
        return false;
    }
    for (auto i : info) {
        fout << i << std::endl;
    }
    if (!fout) {
        fout.close();
        return false;
    }
    fout.close();
    return true;
}

static int RenameFile(const std::string &src, const std::string &des)
{
    return rename(src.c_str(), des.c_str());
}

static void GenerateTestFile()
{
    ASSERT_EQ(true, CopyFile(SEHAP_CONTEXTS_FILE, SEHAP_CONTEXTS_FILE + "_bk"));
    std::vector<std::string> sehapInfo = {
        "apl=system_core name=com.ohos.test domain= type=",
        "apl=system_core name=com.hap.selftest domain=selftest type=selftest_hap_data_file"};
    ASSERT_EQ(true, WriteFile(SEHAP_CONTEXTS_FILE, sehapInfo));

    ASSERT_EQ(true, CopyFile(PARAM_CONTEXTS_FILE, PARAM_CONTEXTS_FILE + "_bk"));
    std::vector<std::string> paramInfo = {"test.para                           u:object_r:testpara:s0"};
    ASSERT_EQ(true, WriteFile(PARAM_CONTEXTS_FILE, paramInfo));
}

static void RemoveTestFile()
{
    ASSERT_EQ(0, RenameFile(SEHAP_CONTEXTS_FILE + "_bk", SEHAP_CONTEXTS_FILE));
    ASSERT_EQ(0, RenameFile(PARAM_CONTEXTS_FILE + "_bk", PARAM_CONTEXTS_FILE));
}

static std::string RunCommand(const std::string &command)
{
    std::string result = "";
    FILE *file = popen(command.c_str(), "r");

    if (file != nullptr) {
        char commandResult[1024] = {0};
        while ((fgets(commandResult, sizeof(commandResult), file)) != nullptr) {
            result.append(commandResult);
        }
        pclose(file);
        file = nullptr;
    }
    return result;
}

void SelinuxUnitTest::SetUpTestCase()
{
    // make test case clean
    GenerateTestFile();
    SetSelinuxLogCallback();
}

void SelinuxUnitTest::TearDownTestCase()
{
    RemoveTestFile();
}

void SelinuxUnitTest::SetUp() {}

void SelinuxUnitTest::TearDown() {}

void SelinuxUnitTest::CreateDataFile() const {}

/**
 * @tc.name: HapFileRestorecon001
 * @tc.desc: HapFileRestorecon input path invalid.
 * @tc.type: FUNC
 * @tc.require:AR000GJSDQ
 */
HWTEST_F(SelinuxUnitTest, HapFileRestorecon001, TestSize.Level1)
{
    ASSERT_EQ(true, CreateDirectory(INVALID_PATH));

    int ret = test.HapFileRestorecon(INVALID_PATH, TEST_APL, TEST_NAME, 0);
    ASSERT_EQ(-SELINUX_PATH_INVAILD, ret);
    ASSERT_EQ(true, RemoveDirectory(INVALID_PATH));

    if (access(NOT_EXIST_PATH.c_str(), F_OK) == 0) {
        ASSERT_EQ(true, RemoveDirectory(NOT_EXIST_PATH));
    }

    ret = test.HapFileRestorecon(NOT_EXIST_PATH, TEST_APL, TEST_NAME, 0);
    ASSERT_EQ(-SELINUX_PATH_INVAILD, ret);
}

/**
 * @tc.name: HapFileRestorecon002
 * @tc.desc: HapFileRestorecon input para empty.
 * @tc.type: FUNC
 * @tc.require:AR000GJSDQ
 */
HWTEST_F(SelinuxUnitTest, HapFileRestorecon002, TestSize.Level1)
{
    ASSERT_EQ(true, CreateDirectory(TEST_SUB_PATH_1));

    int ret = test.HapFileRestorecon("", TEST_APL, TEST_NAME, 0);
    ASSERT_EQ(-SELINUX_ARG_INVALID, ret);

    ret = test.HapFileRestorecon(TEST_SUB_PATH_1, "", TEST_NAME, 0);
    ASSERT_EQ(-SELINUX_ARG_INVALID, ret);

    ret = test.HapFileRestorecon(TEST_SUB_PATH_1, TEST_APL, "", 0);
    ASSERT_EQ(SELINUX_SUCC, ret);

    ASSERT_EQ(true, RemoveDirectory(TEST_PATH));
}

/**
 * @tc.name: HapFileRestorecon003
 * @tc.desc: HapFileRestorecon type empty.
 * @tc.type: FUNC
 * @tc.require:AR000GJSDQ
 */
HWTEST_F(SelinuxUnitTest, HapFileRestorecon003, TestSize.Level1)
{
    ASSERT_EQ(true, CreateDirectory(TEST_SUB_PATH_1));
    // apl=system_core name=com.ohos.test domain= type=
    int ret = test.HapFileRestorecon(TEST_SUB_PATH_1, TEST_APL, "com.ohos.test", 0);
    ASSERT_EQ(-SELINUX_ARG_INVALID, ret);

    ASSERT_EQ(true, RemoveDirectory(TEST_PATH));
}

/**
 * @tc.name: HapFileRestorecon004
 * @tc.desc: HapFileRestorecon input single path no recurse.
 * @tc.type: FUNC
 * @tc.require:AR000GJSDQ
 */
HWTEST_F(SelinuxUnitTest, HapFileRestorecon004, TestSize.Level1)
{
    ASSERT_EQ(true, CreateFile(TEST_SUB_PATH_1_FILE_1)); // this file should not be restorecon

    char *secontextOld = nullptr;
    getfilecon(TEST_SUB_PATH_1_FILE_1.c_str(), &secontextOld);

    int ret = test.HapFileRestorecon(TEST_SUB_PATH_1, TEST_APL, TEST_NAME, 0);
    ASSERT_EQ(SELINUX_SUCC, ret);

    char *secontext = nullptr;
    getfilecon(TEST_SUB_PATH_1.c_str(), &secontext);
    ret = strcmp(DEST_LABEL.c_str(), secontext);
    ASSERT_EQ(SELINUX_SUCC, ret);
    freecon(secontext);
    secontext = nullptr;

    getfilecon(TEST_SUB_PATH_1_FILE_1.c_str(), &secontext);
    ret = strcmp(secontextOld, secontext);
    ASSERT_EQ(SELINUX_SUCC, ret);
    freecon(secontext);
    secontext = nullptr;

    ASSERT_EQ(true, RemoveDirectory(TEST_PATH));
}

/**
 * @tc.name: HapFileRestorecon005
 * @tc.desc: HapFileRestorecon input single path recurse.
 * @tc.type: FUNC
 * @tc.require:AR000GJSDQ
 */
HWTEST_F(SelinuxUnitTest, HapFileRestorecon005, TestSize.Level1)
{
    ASSERT_EQ(true, CreateFile(TEST_SUB_PATH_1_FILE_1));
    ASSERT_EQ(true, CreateFile(TEST_SUB_PATH_1_FILE_2));
    ASSERT_EQ(true, CreateFile(TEST_SUB_PATH_2_FILE_1));
    ASSERT_EQ(true, CreateFile(TEST_SUB_PATH_2_FILE_2));

    int ret = test.HapFileRestorecon(TEST_PATH, TEST_APL, TEST_NAME, 1);
    ASSERT_EQ(SELINUX_SUCC, ret);

    char *secontext = nullptr;
    getfilecon(TEST_SUB_PATH_1.c_str(), &secontext);
    ret = strcmp(DEST_LABEL.c_str(), secontext);
    ASSERT_EQ(SELINUX_SUCC, ret);
    freecon(secontext);
    secontext = nullptr;

    getfilecon(TEST_SUB_PATH_2.c_str(), &secontext);
    ret = strcmp(DEST_LABEL.c_str(), secontext);
    ASSERT_EQ(SELINUX_SUCC, ret);
    freecon(secontext);
    secontext = nullptr;

    getfilecon(TEST_SUB_PATH_1_FILE_1.c_str(), &secontext);
    ret = strcmp(DEST_LABEL.c_str(), secontext);
    ASSERT_EQ(SELINUX_SUCC, ret);
    freecon(secontext);
    secontext = nullptr;

    getfilecon(TEST_SUB_PATH_1_FILE_2.c_str(), &secontext);
    ret = strcmp(DEST_LABEL.c_str(), secontext);
    ASSERT_EQ(SELINUX_SUCC, ret);
    freecon(secontext);
    secontext = nullptr;

    getfilecon(TEST_SUB_PATH_2_FILE_1.c_str(), &secontext);
    ret = strcmp(DEST_LABEL.c_str(), secontext);
    ASSERT_EQ(SELINUX_SUCC, ret);
    freecon(secontext);
    secontext = nullptr;

    getfilecon(TEST_SUB_PATH_2_FILE_2.c_str(), &secontext);
    ret = strcmp(DEST_LABEL.c_str(), secontext);
    ASSERT_EQ(SELINUX_SUCC, ret);
    freecon(secontext);
    secontext = nullptr;

    ASSERT_EQ(true, RemoveDirectory(TEST_PATH));
}

/**
 * @tc.name: HapFileRestorecon006
 * @tc.desc: HapFileRestorecon input single unsimplify path/file.
 * @tc.type: FUNC
 * @tc.require:AR000GJSDQ
 */
HWTEST_F(SelinuxUnitTest, HapFileRestorecon006, TestSize.Level1)
{
    ASSERT_EQ(true, CreateDirectory(TEST_SUB_PATH_4));
    ASSERT_EQ(true, CreateFile(TEST_SUB_PATH_3_FILE_1));

    int ret = test.HapFileRestorecon(TEST_UNSIMPLIFY_PATH, TEST_APL, TEST_NAME, 0);
    ASSERT_EQ(SELINUX_SUCC, ret);

    char *secontext = nullptr;
    getfilecon(TEST_SUB_PATH_4.c_str(), &secontext);
    ret = strcmp(DEST_LABEL.c_str(), secontext);
    ASSERT_EQ(SELINUX_SUCC, ret);
    freecon(secontext);
    secontext = nullptr;

    ret = test.HapFileRestorecon(TEST_UNSIMPLIFY_FILE, TEST_APL, TEST_NAME, 0);
    ASSERT_EQ(SELINUX_SUCC, ret);

    secontext = nullptr;
    getfilecon(TEST_SUB_PATH_3_FILE_1.c_str(), &secontext);
    ret = strcmp(DEST_LABEL.c_str(), secontext);
    ASSERT_EQ(SELINUX_SUCC, ret);
    freecon(secontext);
    secontext = nullptr;

    ASSERT_EQ(true, RemoveDirectory(TEST_PATH));
}

/**
 * @tc.name: HapFileRestorecon007
 * @tc.desc: HapFileRestorecon input multi path/file no recurse.
 * @tc.type: FUNC
 * @tc.require:AR000GJSDQ
 */
HWTEST_F(SelinuxUnitTest, HapFileRestorecon007, TestSize.Level1)
{
    ASSERT_EQ(true, CreateDirectory(TEST_SUB_PATH_4));
    ASSERT_EQ(true, CreateFile(TEST_SUB_PATH_1_FILE_1));
    ASSERT_EQ(true, CreateFile(TEST_SUB_PATH_1_FILE_2));
    ASSERT_EQ(true, CreateFile(TEST_SUB_PATH_2_FILE_1)); // should not be restorecon
    ASSERT_EQ(true, CreateFile(TEST_SUB_PATH_3_FILE_1));

    char *secontextOld = nullptr;
    getfilecon(TEST_SUB_PATH_2_FILE_1.c_str(), &secontextOld);

    std::vector<std::string> tmp;
    tmp.emplace_back(TEST_SUB_PATH_1);
    tmp.emplace_back(TEST_SUB_PATH_2);
    tmp.emplace_back(TEST_SUB_PATH_1_FILE_1);
    tmp.emplace_back(TEST_SUB_PATH_1_FILE_2);
    tmp.emplace_back(TEST_UNSIMPLIFY_FILE);
    tmp.emplace_back(TEST_UNSIMPLIFY_PATH);

    int ret = test.HapFileRestorecon(tmp, TEST_APL, TEST_NAME, 0);
    ASSERT_EQ(SELINUX_SUCC, ret);

    char *secontext = nullptr;
    getfilecon(TEST_SUB_PATH_1.c_str(), &secontext);
    ret = strcmp(DEST_LABEL.c_str(), secontext);
    ASSERT_EQ(SELINUX_SUCC, ret);
    freecon(secontext);
    secontext = nullptr;

    getfilecon(TEST_SUB_PATH_2.c_str(), &secontext);
    ret = strcmp(DEST_LABEL.c_str(), secontext);
    ASSERT_EQ(SELINUX_SUCC, ret);
    freecon(secontext);
    secontext = nullptr;

    getfilecon(TEST_SUB_PATH_1_FILE_1.c_str(), &secontext);
    ret = strcmp(DEST_LABEL.c_str(), secontext);
    ASSERT_EQ(SELINUX_SUCC, ret);
    freecon(secontext);
    secontext = nullptr;

    getfilecon(TEST_SUB_PATH_1_FILE_2.c_str(), &secontext);
    ret = strcmp(DEST_LABEL.c_str(), secontext);
    ASSERT_EQ(SELINUX_SUCC, ret);
    freecon(secontext);
    secontext = nullptr;

    getfilecon(TEST_SUB_PATH_2_FILE_1.c_str(), &secontext); // this file should not be restorecon
    ret = strcmp(secontextOld, secontext);
    ASSERT_EQ(SELINUX_SUCC, ret);
    freecon(secontext);
    freecon(secontextOld);
    secontext = nullptr;
    secontextOld = nullptr;

    getfilecon(TEST_SUB_PATH_3_FILE_1.c_str(), &secontext);
    ret = strcmp(DEST_LABEL.c_str(), secontext);
    ASSERT_EQ(SELINUX_SUCC, ret);
    freecon(secontext);
    secontext = nullptr;

    getfilecon(TEST_SUB_PATH_4.c_str(), &secontext);
    ret = strcmp(DEST_LABEL.c_str(), secontext);
    ASSERT_EQ(SELINUX_SUCC, ret);
    freecon(secontext);
    secontext = nullptr;

    ASSERT_EQ(true, RemoveDirectory(TEST_PATH));
}

/**
 * @tc.name: HapFileRestorecon008
 * @tc.desc: HapFileRestorecon input multi path/file recurse.
 * @tc.type: FUNC
 * @tc.require:AR000GJSDQ
 */
HWTEST_F(SelinuxUnitTest, HapFileRestorecon008, TestSize.Level1)
{
    ASSERT_EQ(true, CreateFile(TEST_SUB_PATH_1_FILE_1));
    ASSERT_EQ(true, CreateFile(TEST_SUB_PATH_1_FILE_2));
    ASSERT_EQ(true, CreateFile(TEST_SUB_PATH_2_FILE_1));
    ASSERT_EQ(true, CreateFile(TEST_SUB_PATH_2_FILE_2));
    ASSERT_EQ(true, CreateFile(TEST_SUB_PATH_3_FILE_1));
    ASSERT_EQ(true, CreateFile(TEST_SUB_PATH_3_FILE_2)); // this file should not be restorecon
    ASSERT_EQ(true, CreateFile(TEST_SUB_PATH_4_FILE_1));
    ASSERT_EQ(true, CreateFile(TEST_SUB_PATH_4_FILE_2));

    std::vector<std::string> tmp;
    tmp.emplace_back(TEST_SUB_PATH_1);
    tmp.emplace_back(TEST_SUB_PATH_2);
    tmp.emplace_back(TEST_UNSIMPLIFY_FILE); // TEST_SUB_PATH_3_FILE_1
    tmp.emplace_back(TEST_UNSIMPLIFY_PATH); // TEST_SUB_PATH_4

    char *secontextOld = nullptr;
    getfilecon(TEST_SUB_PATH_3_FILE_2.c_str(), &secontextOld);

    int ret = test.HapFileRestorecon(tmp, TEST_APL, TEST_NAME, 1);
    ASSERT_EQ(SELINUX_SUCC, ret);

    char *secontext = nullptr;
    getfilecon(TEST_SUB_PATH_1.c_str(), &secontext);
    ret = strcmp(DEST_LABEL.c_str(), secontext);
    ASSERT_EQ(SELINUX_SUCC, ret);
    freecon(secontext);
    secontext = nullptr;

    getfilecon(TEST_SUB_PATH_2.c_str(), &secontext);
    ret = strcmp(DEST_LABEL.c_str(), secontext);
    ASSERT_EQ(SELINUX_SUCC, ret);
    freecon(secontext);
    secontext = nullptr;

    getfilecon(TEST_SUB_PATH_4.c_str(), &secontext);
    ret = strcmp(DEST_LABEL.c_str(), secontext);
    ASSERT_EQ(SELINUX_SUCC, ret);
    freecon(secontext);
    secontext = nullptr;

    getfilecon(TEST_SUB_PATH_1_FILE_1.c_str(), &secontext);
    ret = strcmp(DEST_LABEL.c_str(), secontext);
    ASSERT_EQ(SELINUX_SUCC, ret);
    freecon(secontext);
    secontext = nullptr;

    getfilecon(TEST_SUB_PATH_1_FILE_2.c_str(), &secontext);
    ret = strcmp(DEST_LABEL.c_str(), secontext);
    ASSERT_EQ(SELINUX_SUCC, ret);
    freecon(secontext);
    secontext = nullptr;

    getfilecon(TEST_SUB_PATH_2_FILE_1.c_str(), &secontext);
    ret = strcmp(DEST_LABEL.c_str(), secontext);
    ASSERT_EQ(SELINUX_SUCC, ret);
    freecon(secontext);
    secontext = nullptr;

    getfilecon(TEST_SUB_PATH_2_FILE_2.c_str(), &secontext);
    ret = strcmp(DEST_LABEL.c_str(), secontext);
    ASSERT_EQ(SELINUX_SUCC, ret);
    freecon(secontext);
    secontext = nullptr;

    getfilecon(TEST_SUB_PATH_4_FILE_1.c_str(), &secontext);
    ret = strcmp(DEST_LABEL.c_str(), secontext);
    ASSERT_EQ(SELINUX_SUCC, ret);
    freecon(secontext);
    secontext = nullptr;

    getfilecon(TEST_SUB_PATH_4_FILE_2.c_str(), &secontext);
    ret = strcmp(DEST_LABEL.c_str(), secontext);
    ASSERT_EQ(SELINUX_SUCC, ret);
    freecon(secontext);
    secontext = nullptr;

    getfilecon(TEST_SUB_PATH_3_FILE_1.c_str(), &secontext);
    ret = strcmp(DEST_LABEL.c_str(), secontext);
    ASSERT_EQ(SELINUX_SUCC, ret);
    freecon(secontext);
    secontext = nullptr;

    getfilecon(TEST_SUB_PATH_3_FILE_2.c_str(), &secontext);
    ret = strcmp(secontextOld, secontext);
    ASSERT_EQ(SELINUX_SUCC, ret);
    freecon(secontext);
    freecon(secontextOld);
    secontext = nullptr;
    secontextOld = nullptr;

    ASSERT_EQ(true, RemoveDirectory(TEST_PATH));
}

/**
 * @tc.name: HapFileRestorecon009
 * @tc.desc: HapFileRestorecon repeat label.
 * @tc.type: FUNC
 * @tc.require:AR000GJSDQ
 */
HWTEST_F(SelinuxUnitTest, HapFileRestorecon009, TestSize.Level1)
{
    ASSERT_EQ(true, CreateDirectory(TEST_SUB_PATH_1));

    int ret = test.HapFileRestorecon(TEST_SUB_PATH_1, TEST_APL, TEST_NAME, 0);
    ASSERT_EQ(SELINUX_SUCC, ret);

    char *secontext = nullptr;
    getfilecon(TEST_SUB_PATH_1.c_str(), &secontext);
    ret = strcmp(DEST_LABEL.c_str(), secontext);
    ASSERT_EQ(SELINUX_SUCC, ret);
    freecon(secontext);
    secontext = nullptr;

    char *secontextOld = nullptr;
    getfilecon(TEST_SUB_PATH_1.c_str(), &secontextOld);

    ret = test.HapFileRestorecon(TEST_SUB_PATH_1, TEST_APL, TEST_NAME, 0); // double restorcon
    ASSERT_EQ(SELINUX_SUCC, ret);

    getfilecon(TEST_SUB_PATH_1.c_str(), &secontext);
    ret = strcmp(secontextOld, secontext);
    ASSERT_EQ(SELINUX_SUCC, ret);
    freecon(secontext);
    freecon(secontextOld);
    secontext = nullptr;
    secontextOld = nullptr;

    ASSERT_EQ(true, RemoveDirectory(TEST_PATH));
}

/**
 * @tc.name: HapDomainSetcontext001
 * @tc.desc: HapDomainSetcontext input para empty.
 * @tc.type: FUNC
 * @tc.require:AR000GJSDQ
 */
HWTEST_F(SelinuxUnitTest, HapDomainSetcontext001, TestSize.Level1)
{
    int ret = test.HapDomainSetcontext("", TEST_NAME);
    ASSERT_EQ(-SELINUX_ARG_INVALID, ret);

    ret = test.HapDomainSetcontext(TEST_APL, "");
    ASSERT_EQ(SELINUX_SUCC, ret);
}

/**
 * @tc.name: HapDomainSetcontext002
 * @tc.desc: HapDomainSetcontext domain empty.
 * @tc.type: FUNC
 * @tc.require:AR000GJSDQ
 */
HWTEST_F(SelinuxUnitTest, HapDomainSetcontext002, TestSize.Level1)
{
    // apl=system_core name=com.ohos.test domain= type=
    int ret = test.HapDomainSetcontext(TEST_APL, "com.ohos.test");
    ASSERT_EQ(-SELINUX_ARG_INVALID, ret);
}

/**
 * @tc.name: HapDomainSetcontext003
 * @tc.desc: HapDomainSetcontext domain function test.
 * @tc.type: FUNC
 * @tc.require:AR000GJSDQ
 */
HWTEST_F(SelinuxUnitTest, HapDomainSetcontext003, TestSize.Level1)
{
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0);
    if (pid < 0) {
        return;
    } else if (pid == 0) {
        int ret = test.HapDomainSetcontext(TEST_APL, TEST_NAME);
        ASSERT_EQ(SELINUX_SUCC, ret);
        sleep(SLEEP_SECOND);
        exit(0);
    } else {
        std::string cmdRes = RunCommand("ps -efZ | grep selinux_unittest | grep -v grep");
        ASSERT_TRUE(cmdRes.find(DEST_DOMAIN) != std::string::npos);
    }
}

/**
 * @tc.name: GetParamList001
 * @tc.desc: GetParamList test.
 * @tc.type: FUNC
 * @tc.require:AR000GJSDS
 */
HWTEST_F(SelinuxUnitTest, GetParamList001, TestSize.Level1)
{
    ParamContextsList *buff = nullptr;
    buff = GetParamList();
    ASSERT_NE(nullptr, buff);
    ParamContextsList *head = buff;
    bool find = false;
    while (buff != nullptr) {
        if (std::string(buff->info.paraName) == TEST_PARA_NAME &&
            std::string(buff->info.paraContext) == TEST_PARA_CONTEXT) {
            find = true;
            buff = buff->next;
            continue;
        }
        ASSERT_EQ(SELINUX_SUCC, security_check_context(buff->info.paraContext));
        buff = buff->next;
    }
    ASSERT_EQ(true, find);

    DestroyParamList(&head);
    ASSERT_EQ(nullptr, head);
}

/**
 * @tc.name: DestroyParamList001
 * @tc.desc: DestroyParamList input invalid.
 * @tc.type: FUNC
 * @tc.require:AR000GJSDS
 */
HWTEST_F(SelinuxUnitTest, DestroyParamList001, TestSize.Level1)
{
    DestroyParamList(nullptr);
}

/**
 * @tc.name: GetParamLabel001
 * @tc.desc: GetParamLabel input invalid.
 * @tc.type: FUNC
 * @tc.require:AR000GJSDS
 */
HWTEST_F(SelinuxUnitTest, GetParamLabel001, TestSize.Level1)
{
    char *context = nullptr;
    ASSERT_EQ(-SELINUX_PTR_NULL, GetParamLabel(nullptr, &context));

    ASSERT_EQ(-SELINUX_PTR_NULL, GetParamLabel(TEST_PARA_NAME.c_str(), nullptr));

    for (auto para : TEST_INVALID_PARA) {
        ASSERT_EQ(-SELINUX_ARG_INVALID, GetParamLabel(para.c_str(), &context));
    }

    ASSERT_EQ(SELINUX_SUCC, GetParamLabel(TEST_NOT_EXIST_PARA_NAME.c_str(), &context));
    if (!context) {
        free(context);
    }
}

/**
 * @tc.name: GetParamLabel002
 * @tc.desc: GetParamLabel func test.
 * @tc.type: FUNC
 * @tc.require:AR000GJSDS
 */
HWTEST_F(SelinuxUnitTest, GetParamLabel002, TestSize.Level1)
{
    char *context = nullptr;
    ASSERT_EQ(SELINUX_SUCC, GetParamLabel(TEST_PARA_NAME.c_str(), &context));
    ASSERT_EQ(TEST_PARA_CONTEXT, std::string(context));
    if (!context) {
        free(context);
    }
}

/**
 * @tc.name: ReadParamCheck001
 * @tc.desc: ReadParamCheck input invalid.
 * @tc.type: FUNC
 * @tc.require:AR000GJSDS
 */
HWTEST_F(SelinuxUnitTest, ReadParamCheck001, TestSize.Level1)
{
    ASSERT_EQ(-SELINUX_PTR_NULL, ReadParamCheck(nullptr));

    for (auto para : TEST_INVALID_PARA) {
        ASSERT_EQ(-SELINUX_ARG_INVALID, ReadParamCheck(para.c_str()));
    }

    ASSERT_EQ(SELINUX_SUCC, ReadParamCheck(TEST_NOT_EXIST_PARA_NAME.c_str()));

    std::string cmd = "dmesg | grep 'avc:  denied  { read } for parameter=" + TEST_NOT_EXIST_PARA_NAME +
                      " pid=" + std::to_string(getpid()) + "' | grep 'tcontext=" + DEFAULT_PARA_CONTEXT +
                      " tclass=file'";
    std::string cmdRes = RunCommand(cmd);
    ASSERT_TRUE(cmdRes.find(TEST_NOT_EXIST_PARA_NAME) != std::string::npos);
}

/**
 * @tc.name: ReadParamCheck002
 * @tc.desc: ReadParamCheck func test.
 * @tc.type: FUNC
 * @tc.require:AR000GJSDS
 */
HWTEST_F(SelinuxUnitTest, ReadParamCheck002, TestSize.Level1)
{
    ASSERT_EQ(SELINUX_SUCC, ReadParamCheck(TEST_PARA_NAME.c_str()));
    std::string cmd = "dmesg | grep 'avc:  denied  { read } for parameter=" + TEST_PARA_NAME +
                      " pid=" + std::to_string(getpid()) + "' | grep 'tcontext=" + TEST_PARA_CONTEXT + " tclass=file'";
    std::string cmdRes = RunCommand(cmd);
    ASSERT_TRUE(cmdRes.find(TEST_PARA_NAME) != std::string::npos);
}

/**
 * @tc.name: SetParamCheck001
 * @tc.desc: SetParamCheck input invalid.
 * @tc.type: FUNC
 * @tc.require:AR000GJSDS
 */
HWTEST_F(SelinuxUnitTest, SetParamCheck001, TestSize.Level1)
{
    struct ucred uc;
    uc.pid = getpid();
    uc.uid = getuid();
    uc.gid = getgid();
    ASSERT_EQ(-SELINUX_PTR_NULL, SetParamCheck(nullptr, &uc));

    ASSERT_EQ(-SELINUX_PTR_NULL, SetParamCheck(TEST_NOT_EXIST_PARA_NAME.c_str(), nullptr));

    for (auto para : TEST_INVALID_PARA) {
        ASSERT_EQ(-SELINUX_ARG_INVALID, SetParamCheck(para.c_str(), &uc));
    }

    ASSERT_EQ(SELINUX_SUCC, SetParamCheck(TEST_NOT_EXIST_PARA_NAME.c_str(), &uc));
    std::string cmd = "dmesg | grep 'avc:  denied  { set } for parameter=" + TEST_NOT_EXIST_PARA_NAME +
                      " pid=" + std::to_string(getpid()) + "' | grep 'tcontext=" + DEFAULT_PARA_CONTEXT +
                      " tclass=parameter_service'";
    std::string cmdRes = RunCommand(cmd);
    ASSERT_TRUE(cmdRes.find(TEST_NOT_EXIST_PARA_NAME) != std::string::npos);
}

/**
 * @tc.name: SetParamCheck002
 * @tc.desc: SetParamCheck func test.
 * @tc.type: FUNC
 * @tc.require:AR000GJSDS
 */
HWTEST_F(SelinuxUnitTest, SetParamCheck002, TestSize.Level1)
{
    struct ucred uc;
    uc.pid = getpid();
    uc.uid = getuid();
    uc.gid = getgid();
    ASSERT_EQ(SELINUX_SUCC, SetParamCheck(TEST_PARA_NAME.c_str(), &uc));
    std::string cmd = "dmesg | grep 'avc:  denied  { set } for parameter=" + TEST_PARA_NAME +
                      " pid=" + std::to_string(getpid()) + "' | grep 'tcontext=" + TEST_PARA_CONTEXT +
                      " tclass=parameter_service'";
    std::string cmdRes = RunCommand(cmd);
    ASSERT_TRUE(cmdRes.find(TEST_PARA_NAME) != std::string::npos);
}

/**
 * @tc.name: HdfListServiceCheck001
 * @tc.desc: HdfListServiceCheck test.
 * @tc.type: FUNC
 * @tc.require:AR000GJSDS
 */
HWTEST_F(SelinuxUnitTest, HdfListServiceCheck001, TestSize.Level1)
{
    ASSERT_EQ(-SELINUX_GET_CONTEXT_ERROR, HdfListServiceCheck(-1));
    ASSERT_EQ(SELINUX_SUCC, HdfListServiceCheck(getpid()));
    std::string cmd =
        "dmesg | grep 'avc:  denied  { list } for service=hdf_devmgr_class pid=" + std::to_string(getpid()) +
        "' | grep 'tclass=hdf_devmgr_class'";
    std::string cmdRes = RunCommand(cmd);
    ASSERT_TRUE(cmdRes.find("hdf_devmgr_class") != std::string::npos);
}

/**
 * @tc.name: HdfGetServiceCheck001
 * @tc.desc: HdfGetServiceCheck test.
 * @tc.type: FUNC
 * @tc.require:AR000GJSDS
 */
HWTEST_F(SelinuxUnitTest, HdfGetServiceCheck001, TestSize.Level1)
{
    ASSERT_EQ(-SELINUX_GET_CONTEXT_ERROR, HdfGetServiceCheck(-1, TEST_SERVICE_NAME.c_str()));
    ASSERT_EQ(-SELINUX_PTR_NULL, HdfGetServiceCheck(getpid(), nullptr));
    ASSERT_EQ(SELINUX_SUCC, HdfGetServiceCheck(getpid(), TEST_SERVICE_NAME.c_str()));
    std::string cmd = "dmesg | grep 'avc:  denied  { get } for service=" + TEST_SERVICE_NAME +
                      " pid=" + std::to_string(getpid()) + "' | grep 'tclass=hdf_devmgr_class'";
    std::string cmdRes = RunCommand(cmd);
    ASSERT_TRUE(cmdRes.find(TEST_SERVICE_NAME) != std::string::npos);
}

/**
 * @tc.name: HdfAddServiceCheck001
 * @tc.desc: HdfAddServiceCheck test.
 * @tc.type: FUNC
 * @tc.require:AR000GJSDS
 */
HWTEST_F(SelinuxUnitTest, HdfAddServiceCheck001, TestSize.Level1)
{
    ASSERT_EQ(-SELINUX_GET_CONTEXT_ERROR, HdfAddServiceCheck(-1, TEST_SERVICE_NAME.c_str()));
    ASSERT_EQ(-SELINUX_PTR_NULL, HdfAddServiceCheck(getpid(), nullptr));
    ASSERT_EQ(SELINUX_SUCC, HdfAddServiceCheck(getpid(), TEST_SERVICE_NAME.c_str()));
    std::string cmd = "dmesg | grep 'avc:  denied  { add } for service=" + TEST_SERVICE_NAME +
                      " pid=" + std::to_string(getpid()) + "' | grep 'tclass=hdf_devmgr_class'";
    std::string cmdRes = RunCommand(cmd);
    ASSERT_TRUE(cmdRes.find(TEST_SERVICE_NAME) != std::string::npos);
}

/**
 * @tc.name: ListServiceCheck001
 * @tc.desc: ListServiceCheck test.
 * @tc.type: FUNC
 * @tc.require:AR000GJSDS
 */
HWTEST_F(SelinuxUnitTest, ListServiceCheck001, TestSize.Level1)
{
    ServiceChecker service(false);
    ASSERT_EQ(-SELINUX_GET_CONTEXT_ERROR, service.ListServiceCheck(-1));
    ASSERT_EQ(SELINUX_SUCC, service.ListServiceCheck(getpid()));
    std::string cmd = "dmesg | grep 'avc:  denied  { list } for service=samgr_class pid=" + std::to_string(getpid()) +
                      "' | grep 'tclass=samgr_class'";
    std::string cmdRes = RunCommand(cmd);
    ASSERT_TRUE(cmdRes.find("samgr_class") != std::string::npos);
}

/**
 * @tc.name: GetServiceCheck001
 * @tc.desc: GetServiceCheck test.
 * @tc.type: FUNC
 * @tc.require:AR000GJSDS
 */
HWTEST_F(SelinuxUnitTest, GetServiceCheck001, TestSize.Level1)
{
    ServiceChecker service(false);
    ASSERT_EQ(-SELINUX_GET_CONTEXT_ERROR, service.GetServiceCheck(-1, TEST_SERVICE_NAME));
    ASSERT_EQ(-SELINUX_ARG_INVALID, service.GetServiceCheck(getpid(), ""));
    ASSERT_EQ(SELINUX_SUCC, service.GetServiceCheck(getpid(), TEST_SERVICE_NAME));
    std::string cmd = "dmesg | grep 'avc:  denied  { get } for service=" + TEST_SERVICE_NAME +
                      " pid=" + std::to_string(getpid()) + "' | grep 'tclass=samgr_class'";
    std::string cmdRes = RunCommand(cmd);
    ASSERT_TRUE(cmdRes.find(TEST_SERVICE_NAME) != std::string::npos);
}

/**
 * @tc.name: GetRemoteServiceCheck001
 * @tc.desc: GetRemoteServiceCheck test.
 * @tc.type: FUNC
 * @tc.require:AR000GJSDS
 */
HWTEST_F(SelinuxUnitTest, GetRemoteServiceCheck001, TestSize.Level1)
{
    ServiceChecker service(false);
    ASSERT_EQ(-SELINUX_GET_CONTEXT_ERROR, service.GetRemoteServiceCheck(-1, TEST_SERVICE_NAME));
    ASSERT_EQ(-SELINUX_ARG_INVALID, service.GetRemoteServiceCheck(getpid(), ""));
    ASSERT_EQ(SELINUX_SUCC, service.GetRemoteServiceCheck(getpid(), TEST_SERVICE_NAME));
    std::string cmd = "dmesg | grep 'avc:  denied  { get_remote } for service=" + TEST_SERVICE_NAME +
                      " pid=" + std::to_string(getpid()) + "' | grep 'tclass=samgr_class'";
    std::string cmdRes = RunCommand(cmd);
    ASSERT_TRUE(cmdRes.find(TEST_SERVICE_NAME) != std::string::npos);
}

/**
 * @tc.name: AddServiceCheck001
 * @tc.desc: AddServiceCheck test.
 * @tc.type: FUNC
 * @tc.require:AR000GJSDS
 */
HWTEST_F(SelinuxUnitTest, AddServiceCheck001, TestSize.Level1)
{
    ServiceChecker service(false);
    ASSERT_EQ(-SELINUX_GET_CONTEXT_ERROR, service.AddServiceCheck(-1, TEST_SERVICE_NAME));
    ASSERT_EQ(-SELINUX_ARG_INVALID, service.AddServiceCheck(getpid(), ""));
    ASSERT_EQ(SELINUX_SUCC, service.AddServiceCheck(getpid(), TEST_SERVICE_NAME));
    std::string cmd = "dmesg | grep 'avc:  denied  { add } for service=" + TEST_SERVICE_NAME +
                      " pid=" + std::to_string(getpid()) + "' | grep 'tclass=samgr_class'";
    std::string cmdRes = RunCommand(cmd);
    ASSERT_TRUE(cmdRes.find(TEST_SERVICE_NAME) != std::string::npos);
}
