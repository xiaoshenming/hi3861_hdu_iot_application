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

#ifndef HISYSEVENT_TOOL_H
#define HISYSEVENT_TOOL_H

#include <string>
#include <thread>

#include "hisysevent_manager.h"

namespace OHOS {
namespace HiviewDFX {
struct ArgStuct {
    bool real;
    std::string domain;
    std::string eventName;
    std::string tag;
    RuleType ruleType;
    bool history;
    bool isDebug;
    long long beginTime;
    long long endTime;
    int maxEvents;
};

class HiSysEventTool {
public:
    HiSysEventTool();
    bool ParseCmdLine(int argc, char** argv);
    void DoCmdHelp();
    void WaitClient();
    void NotifyClient();
    bool DoAction();
    ~HiSysEventTool() {}

private:
    bool CheckCmdLine();

    struct ArgStuct clientCmdArg;
    std::mutex mutexClient;
    std::condition_variable condvClient;
};
} // namespace HiviewDFX
} // namespace OHOS

#endif // HISYSEVENT_TOOL_H
