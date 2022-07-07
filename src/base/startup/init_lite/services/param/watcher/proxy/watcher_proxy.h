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

#ifndef STARTUP_WATCHER_PROXY_H
#define STARTUP_WATCHER_PROXY_H

#include "iremote_proxy.h"
#include "iwatcher.h"

namespace OHOS {
namespace init_param {
class WatcherProxy : public IRemoteProxy<IWatcher> {
public:
    explicit WatcherProxy(const sptr<IRemoteObject>& impl) : IRemoteProxy<IWatcher>(impl) {}

    virtual ~WatcherProxy() = default;

    void OnParamerterChange(const std::string &name, const std::string &value) override;

private:
    static inline BrokerDelegator<WatcherProxy> delegator_;
};
}
} // namespace OHOS
#endif // STARTUP_WATCHER_PROXY_H