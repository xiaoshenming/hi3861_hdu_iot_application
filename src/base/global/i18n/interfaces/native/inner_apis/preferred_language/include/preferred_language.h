/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#ifndef OHOS_GLOBAL_I18N_PREFERRED_LANGUAGE_H
#define OHOS_GLOBAL_I18N_PREFERRED_LANGUAGE_H

#include <set>
#include <string>
#include <vector>

namespace OHOS {
namespace Global {
namespace I18n {
class PreferredLanguage {
public:
    static bool AddPreferredLanguage(const std::string& language, int index);
    static bool RemovePreferredLanguage(int index);
    static std::vector<std::string> GetPreferredLanguageList();
    static std::string GetFirstPreferredLanguage();
    static std::string GetPreferredLocale();

private:
    static bool AddPreferredLanguageNonExist(std::vector<std::string> &preferredLangList, int index,
                                             const std::string& language);
    static bool AddPreferredLanguageExist(std::vector<std::string> &preferredLangList, int languageIdx, int index,
                                          const std::string& language);
    static bool IsMatched(const std::string& preferred, const std::string& resource);
    static bool IsValidLanguage(const std::string &language);
    static bool IsValidTag(const std::string &tag);
    static void Split(const std::string &src, const std::string &sep, std::vector<std::string> &dest);
    static const char *RESOURCE_PATH_HEAD;
    static const char *RESOURCE_PATH_TAILOR;
    static const char *RESOURCE_PATH_SPLITOR;
    static const char *PREFERRED_LANGUAGES;
    static constexpr int CONFIG_LEN = 128;
    static constexpr uint32_t LANGUAGE_LEN = 2;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif