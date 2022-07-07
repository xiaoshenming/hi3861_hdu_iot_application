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

#ifndef HISYSEVENT_SUBSCRIBE_OPERATE_H
#define HISYSEVENT_SUBSCRIBE_OPERATE_H

#include <memory>
#include <string>
#include <vector>

#include "hisysevent_manager.h"
#include "hisysevent_subscribe_callback.h"

namespace OHOS {
namespace HiviewDFX {
class HiSysEventSubscribeOperate {
public:
    HiSysEventSubscribeOperate() = default;
    virtual ~HiSysEventSubscribeOperate() {}

public:
    virtual bool AddEventListener(const std::shared_ptr<HiSysEventSubscribeCallBack> listener,
        const std::vector<ListenerRule>& Rule);
    virtual bool RemoveListener(const std::shared_ptr<HiSysEventSubscribeCallBack> listener);
    virtual bool SetDebugMode(const std::shared_ptr<HiSysEventSubscribeCallBack> listener, const bool mode);

private:
    HiSysEventSubscribeOperate(const HiSysEventSubscribeOperate&) = delete;
    HiSysEventSubscribeOperate& operator=(const HiSysEventSubscribeOperate&) = delete;
    HiSysEventSubscribeOperate(const HiSysEventSubscribeOperate&&) = delete;
    HiSysEventSubscribeOperate& operator=(const HiSysEventSubscribeOperate&&) = delete;
};
} // namespace HiviewDFX
} // namespace OHOS

#endif // HISYSEVENT_SUBSCRIBE_OPERATE_H
