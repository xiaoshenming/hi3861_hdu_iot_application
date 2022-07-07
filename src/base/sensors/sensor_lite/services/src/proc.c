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

#include <stdio.h>
#include <unistd.h>
#include "samgr_lite.h"

void __attribute__((weak)) OHOS_SystemInit(void)
{
    SAMGR_Bootstrap();
};

int main()
{
    OHOS_SystemInit();
    // pause only returns when a signal was caught and the signal-catching function returned.
    // pause only returns -1, no need to process the return value.
    (void)pause();
}