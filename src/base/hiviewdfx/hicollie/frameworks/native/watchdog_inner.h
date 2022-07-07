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

#ifndef RELIABILITY_WATCHDOG_INNER_H
#define RELIABILITY_WATCHDOG_INNER_H

#include <atomic>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <string>
#include <thread>
#include <unordered_map>

#include "handler_checker.h"
#include "singleton.h"

namespace OHOS {
namespace HiviewDFX {
class WatchdogInner : public Singleton<WatchdogInner> {
    DECLARE_SINGLETON(WatchdogInner);
public:
    int AddThread(const std::string& name, std::shared_ptr<AppExecFwk::EventHandler> handler,
        unsigned int timeval);
private:
    void SetTimeval(unsigned int timeval);
    unsigned int GetCheckInterval() const;
    int EvaluateCheckerState();
    std::string GetBlockDescription(unsigned int interval);
    void SendEvent(const std::string &keyMsg) const;
    bool Start();
    bool Stop();

    static const unsigned int MAX_WATCH_NUM = 128; // 128: max handler thread
    unsigned int timeval_ = 0;
    std::unordered_map<std::string, std::shared_ptr<HandlerChecker>> handlerMap_;
    std::unique_ptr<std::thread> threadLoop_;
    std::mutex lock_;
    std::condition_variable condition_;
    std::atomic_bool isNeedStop_ = false;
};
} // end of namespace HiviewDFX
} // end of namespace OHOS
#endif
