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
#ifndef OHOS_GLOBAL_I18N_BREAKITERATOR_H
#define OHOS_GLOBAL_I18N_BREAKITERATOR_H

#include "unicode/brkiter.h"

namespace OHOS {
namespace Global {
namespace I18n {
class I18nBreakIterator {
public:
    explicit I18nBreakIterator(std::string lcoaleTag);
    virtual ~I18nBreakIterator();
    int32_t current();
    int32_t first();
    int32_t last();
    int32_t next(int32_t number);
    int32_t next();
    int32_t previous();
    int32_t following(int32_t offset);
    void setText(const char* text);
    void getText(std::string &str);
    bool isBoundary(int32_t offset);
private:
    icu::BreakIterator *iter = nullptr;
    icu::UnicodeString ftext = "";
    static constexpr int32_t OFF_BOUND = -1;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif