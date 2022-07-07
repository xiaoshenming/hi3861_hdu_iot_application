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

#ifndef HISYSEVENT_SUBSCRIBE_CALLBACK_H
#define HISYSEVENT_SUBSCRIBE_CALLBACK_H

#include <string>
#include <memory>

namespace OHOS {
namespace HiviewDFX {
class HiSysEventSubscribeOperate;
class HiSysEventSubscribeCallBack {
public:
    HiSysEventSubscribeCallBack() {}
    virtual void OnHandle(const std::string& domain, const std::string& eventName, const int eventType,
        const std::string& eventDetail);
    virtual void OnServiceDied();
    virtual ~HiSysEventSubscribeCallBack() {}

private:
    HiSysEventSubscribeCallBack(const HiSysEventSubscribeCallBack&) = delete;
    HiSysEventSubscribeCallBack& operator=(const HiSysEventSubscribeCallBack&) = delete;
    HiSysEventSubscribeCallBack(const HiSysEventSubscribeCallBack&&) = delete;
    HiSysEventSubscribeCallBack& operator=(const HiSysEventSubscribeCallBack&&) = delete;

    HiSysEventSubscribeOperate* listenerProxy = nullptr;

friend class HiSysEventManager;
};
} // namespace HiviewDFX
} // namespace OHOS

#endif // HISYSEVENT_SUBSCRIBE_CALLBACK_BASE_H
