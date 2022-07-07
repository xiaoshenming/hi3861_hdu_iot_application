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

#ifndef HISYSEVENT_DELEGATE_H
#define HISYSEVENT_DELEGATE_H

#include <mutex>
#include <string>
#include <thread>

#include "hisysevent_listener_proxy.h"
#include "hisysevent_manager.h"
#include "hisysevent_query_callback.h"
#include "hisysevent_subscribe_callback.h"
#include "hisysevent_subscribe_operate.h"
#include "sys_event_query_rule.h"
#include "system_ability.h"
#include "system_ability_definition.h"
#include "sys_event_rule.h"

namespace OHOS {
namespace HiviewDFX {
class HiSysEventDelegate : public HiSysEventSubscribeOperate {
public:
    HiSysEventDelegate() {}
    bool AddEventListener(const std::shared_ptr<HiSysEventSubscribeCallBack> listener,
        const std::vector<ListenerRule>& rules);
    bool RemoveListener(const std::shared_ptr<HiSysEventSubscribeCallBack> listener);
    bool QueryHiSysEvent(const struct QueryArg& queryArg,
        const std::vector<QueryRule>& queryRules,
        const std::shared_ptr<HiSysEventQueryCallBack> queryCallBack) const;
    bool SetDebugMode(const std::shared_ptr<HiSysEventSubscribeCallBack> listener,
        const bool mode);
    virtual ~HiSysEventDelegate();
    static void BinderFunc();

private:
    void ConvertListenerRule(const std::vector<ListenerRule>& rules,
        std::vector<SysEventRule>& sysRules) const;
    void ConvertQueryRule(const std::vector<QueryRule>& rules,
        std::vector<SysEventQueryRule>& sysRules) const;
    sptr<IRemoteObject> GetSysEventService() const;
    sptr<OHOS::HiviewDFX::HiSysEventListenerProxy> spListenerCallBack = nullptr;
};
} // namespace HiviewDFX
} // namespace OHOS

#endif // HISYSEVENT_DELEGATE_H
