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

#include "hisysevent_query_proxy.h"

#include "string_convertor.h"

namespace OHOS {
namespace HiviewDFX {
void HiSysEventQueryProxy::OnQuery(const ::std::vector<std::u16string>& sysEvent,
    const ::std::vector<int64_t>& seq)
{
    if (queryListener != nullptr) {
        std::vector<std::string> strList;
        for_each(sysEvent.cbegin(), sysEvent.cend(), [&strList](const std::u16string& tmp) {
            strList.emplace_back(U16String2String(tmp));
        });
        queryListener->OnQuery(strList, seq);
    }
}

void HiSysEventQueryProxy::OnComplete(int32_t reason, int32_t total)
{
    if (queryListener != nullptr) {
        queryListener->OnComplete(reason, total);
    }
}
} // namespace HiviewDFX
} // namespace OHOS
