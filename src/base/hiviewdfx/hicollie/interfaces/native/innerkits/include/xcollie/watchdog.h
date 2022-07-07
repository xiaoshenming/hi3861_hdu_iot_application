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

#ifndef RELIABILITY_WATCHDOG_H
#define RELIABILITY_WATCHDOG_H

#include <string>
#include "event_handler.h"
#include "singleton.h"

namespace OHOS {
namespace HiviewDFX {
class Watchdog : public Singleton<Watchdog> {
    DECLARE_SINGLETON(Watchdog);
public:

    /**
     * Add current handler to watchdog thread
     *
     * @param name of current thread
     * @param current EventHandler
     * @param time threshold to kill the process
     * @return 0 if added
     */
    int AddThread(const std::string &name, std::shared_ptr<AppExecFwk::EventHandler> handler,
        unsigned int timeval);
};
} // end of namespace HiviewDFX
} // end of namespace OHOS
#endif

