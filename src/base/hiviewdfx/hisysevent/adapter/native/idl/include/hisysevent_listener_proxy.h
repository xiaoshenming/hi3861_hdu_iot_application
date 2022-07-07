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

#ifndef HISYSEVENT_LISTENER_PROXY_H
#define HISYSEVENT_LISTENER_PROXY_H

#include <string>
#include <vector>

#include "hisysevent_subscribe_callback.h"
#include "sys_event_callback_stub.h"

namespace OHOS {
namespace HiviewDFX {
class CallbackDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    explicit CallbackDeathRecipient(const std::shared_ptr<HiSysEventSubscribeCallBack> callback)
        : subScribeListener(callback) {}
    virtual ~CallbackDeathRecipient() {}
    void OnRemoteDied(const wptr<IRemoteObject> &object) override;
    std::shared_ptr<HiSysEventSubscribeCallBack> GetSubScribeListener() const;

private:
    std::shared_ptr<HiSysEventSubscribeCallBack> subScribeListener;
};

class HiSysEventListenerProxy : public SysEventCallbackStub {
public:
    explicit HiSysEventListenerProxy(const std::shared_ptr<HiSysEventSubscribeCallBack> callback)
        : callbackDeathRecipient(new CallbackDeathRecipient(callback)) {}
    void Handle(const std::u16string& domain, const std::u16string& eventName, uint32_t eventType,
        const std::u16string& eventDetail) override;
    sptr<CallbackDeathRecipient> GetCallbackDeathRecipient() const;
    std::shared_ptr<HiSysEventSubscribeCallBack> GetSubScribeListener() const;
    virtual ~HiSysEventListenerProxy() {}
private:
    sptr<CallbackDeathRecipient> callbackDeathRecipient;
};
} // namespace HiviewDFX
} // namespace OHOS

#endif // HISYSEVENT_LISTENER_PROXY_H
