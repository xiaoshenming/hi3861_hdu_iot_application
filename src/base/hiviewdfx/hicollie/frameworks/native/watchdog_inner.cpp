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

#include "watchdog_inner.h"
#include <unistd.h>

#include "hisysevent.h"
#include "xcollie_utils.h"

namespace OHOS {
namespace HiviewDFX {
WatchdogInner::WatchdogInner()
{
}

WatchdogInner::~WatchdogInner()
{
    Stop();
}

int WatchdogInner::AddThread(const std::string& name, std::shared_ptr<AppExecFwk::EventHandler> handler,
    unsigned int timeval)
{
    if (name.empty() || handler == nullptr) {
        XCOLLIE_LOGE("Add thread fail, invalid args!");
        return -1;
    }
    std::unique_lock<std::mutex> lock(lock_);
    SetTimeval(timeval);
    for (auto [k, v] : handlerMap_) {
        if (k == name || v->GetHandler() == handler) {
            return 0;
        }
    }
    if (handlerMap_.size() >= MAX_WATCH_NUM) {
        return -1;
    }
    auto checker = std::make_shared<HandlerChecker>(name, handler);
    if (checker == nullptr) {
        return -1;
    }
    handlerMap_.insert(std::make_pair(name, checker));
    if (threadLoop_ == nullptr) {
        threadLoop_ = std::make_unique<std::thread>(&WatchdogInner::Start, this);
        XCOLLIE_LOGI("Watchdog is running!");
    }
    XCOLLIE_LOGI("Add thread success : %{public}s, timethreshold : %{public}d", name.c_str(), timeval);
    return 0;
}

bool WatchdogInner::Start()
{
    XCOLLIE_LOGI("Run watchdog!");
    while (!isNeedStop_) {
        std::unique_lock lock(lock_);

        // send
        for (auto info : handlerMap_) {
            info.second->ScheduleCheck();
        }

        // sleep
        unsigned int interval = GetCheckInterval();
        if (condition_.wait_for(lock, std::chrono::milliseconds(interval)) !=  std::cv_status::timeout) {
            XCOLLIE_LOGE("Watchdog is exiting");
            break;
        }

        // check
        int waitState = EvaluateCheckerState();
        if (waitState == CheckStatus::COMPLETED) {
            continue;
        } else if (waitState == CheckStatus::WAITING) {
            XCOLLIE_LOGI("Watchdog half-block happened, send event");
            std::string description = GetBlockDescription(interval / 1000); // 1s = 1000ms
            SendEvent(description);
        } else {
            XCOLLIE_LOGI("Watchdog happened, send event twice, and skip exiting process");
            std::string description = GetBlockDescription(interval / 1000) + ", report twice instead of exiting process."; // 1s = 1000ms
            SendEvent(description);
        }
    }
    return true;
}

int WatchdogInner::EvaluateCheckerState()
{
    int state = CheckStatus::COMPLETED;
    for (auto info : handlerMap_) {
        state = std::max(state, info.second->GetCheckState());
    }
    return state;
}

std::string WatchdogInner::GetBlockDescription(unsigned int interval)
{
    std::string desc = "Watchdog: thread(";
    for (auto info : handlerMap_) {
        if (info.second->GetCheckState() > CheckStatus::COMPLETED) {
            desc += info.first + " ";
        }
    }
    desc += ") blocked " + std::to_string(interval) + "s";
    return desc;
}

bool WatchdogInner::Stop()
{
    isNeedStop_.store(true);
    condition_.notify_all();

    if (threadLoop_ != nullptr && threadLoop_->joinable()) {
        threadLoop_->join();
        threadLoop_ = nullptr;
    }
    return true;
}

void WatchdogInner::SetTimeval(unsigned int timeval)
{
    timeval_ = timeval;
}

unsigned int WatchdogInner::GetCheckInterval() const
{
    return timeval_ * 500; // 0.5s = 500ms
}


void WatchdogInner::SendEvent(const std::string &keyMsg) const
{
    pid_t pid = getpid();
    gid_t gid = getgid();
    time_t curTime = time(nullptr);
    std::string sendMsg = std::string((ctime(&curTime) == nullptr) ? "" : ctime(&curTime)) +
        "\n" + keyMsg;
    HiSysEvent::Write("FRAMEWORK", "SERVICE_BLOCK", HiSysEvent::EventType::FAULT,
        "PID", pid, "TGID", gid, "MSG", sendMsg);
    XCOLLIE_LOGI("send event [FRAMEWORK,SERVICE_BLOCK], msg=%s", keyMsg.c_str());
}
} // end of namespace HiviewDFX
} // end of namespace OHOS
