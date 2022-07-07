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
#include <cerrno>
#include <unistd.h>
#include "fs_manager/fs_manager.h"
#include "init_unittest.h"
#include "init_mount.h"
#include "securec.h"
using namespace std;
using namespace testing::ext;

namespace init_ut {
class MountUnitTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
    void SetUp() {};
    void TearDown() {};
};

HWTEST_F(MountUnitTest, TestMountRequriedPartitions, TestSize.Level0)
{
    const char *fstabFiles = "/etc/fstab.required";
    Fstab *fstab = NULL;
    fstab = ReadFstabFromFile(fstabFiles, false);
    if (fstab != NULL) {
        int ret = MountRequriedPartitions(fstab);
        EXPECT_EQ(ret, -1);
        ReleaseFstab(fstab);
    }
}
} // namespace init_ut
