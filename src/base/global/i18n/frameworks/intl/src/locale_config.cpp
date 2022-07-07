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
#include <regex>
#include "accesstoken_kit.h"
#ifdef TEL_CORE_SERVICE_EXISTS
#include "core_service_client.h"
#endif
#include "hilog/log.h"
#include "ipc_skeleton.h"
#include "libxml/parser.h"
#include "locale_info.h"
#include "localebuilder.h"
#include "locid.h"
#include "ohos/init_data.h"
#include "parameter.h"
#include "securec.h"
#include "string_ex.h"
#include "ucase.h"
#include "ulocimp.h"
#include "unistr.h"
#include "ureslocs.h"
#include "ustring.h"
#include "ustr_imp.h"
#include "locale_config.h"

namespace OHOS {
namespace Global {
namespace I18n {
using namespace std;
using namespace OHOS::HiviewDFX;

static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0xD001E00, "LocaleConfig" };
const char *LocaleConfig::LANGUAGE_KEY = "persist.global.language";
const char *LocaleConfig::LOCALE_KEY = "persist.global.locale";
const char *LocaleConfig::HOUR_KEY = "persist.global.is24Hour";
const char *LocaleConfig::DEFAULT_LOCALE_KEY = "const.global.locale";
const char *LocaleConfig::DEFAULT_LANGUAGE_KEY = "const.global.language";
const char *LocaleConfig::DEFAULT_REGION_KEY = "const.global.region";
const char *LocaleConfig::SUPPORTED_LOCALES_NAME = "supported_locales";
const char *LocaleConfig::SUPPORTED_REGIONS_NAME = "supported_regions";
const char *LocaleConfig::WHITE_LANGUAGES_NAME = "white_languages";
const char *LocaleConfig::FORBIDDEN_LANGUAGES_NAME = "forbidden_languages";
const char *LocaleConfig::FORBIDDEN_REGIONS_NAME = "forbidden_regions";
const char *LocaleConfig::FORBIDDEN_LANGUAGES_PATH = "/system/usr/ohos_locale_config/forbidden_languages.xml";
const char *LocaleConfig::FORBIDDEN_REGIONS_PATH = "/system/usr/ohos_locale_config/forbidden_regions.xml";
const char *LocaleConfig::SUPPORTED_LOCALES_PATH = "/system/usr/ohos_locale_config/supported_locales.xml";
const char *LocaleConfig::SUPPORTED_REGIONS_PATH = "/system/usr/ohos_locale_config/supported_regions.xml";
const char *LocaleConfig::WHITE_LANGUAGES_PATH = "/system/usr/ohos_locale_config/white_languages.xml";
unordered_set<string> LocaleConfig::supportedLocales;
unordered_set<string> LocaleConfig::supportedRegions;
unordered_set<string> LocaleConfig::whiteLanguages;
unordered_map<string, string> LocaleConfig::dialectMap {
    { "es-Latn-419", "es-Latn-419" },
    { "es-Latn-BO", "es-Latn-419" },
    { "es-Latn-BR", "es-Latn-419" },
    { "es-Latn-BZ", "es-Latn-419" },
    { "es-Latn-CL", "es-Latn-419" },
    { "es-Latn-CO", "es-Latn-419" },
    { "es-Latn-CR", "es-Latn-419" },
    { "es-Latn-CU", "es-Latn-419" },
    { "es-Latn-DO", "es-Latn-419" },
    { "es-Latn-EC", "es-Latn-419" },
    { "es-Latn-GT", "es-Latn-419" },
    { "es-Latn-HN", "es-Latn-419" },
    { "es-Latn-MX", "es-Latn-419" },
    { "es-Latn-NI", "es-Latn-419" },
    { "es-Latn-PA", "es-Latn-419" },
    { "es-Latn-PE", "es-Latn-419" },
    { "es-Latn-PR", "es-Latn-419" },
    { "es-Latn-PY", "es-Latn-419" },
    { "es-Latn-SV", "es-Latn-419" },
    { "es-Latn-US", "es-Latn-419" },
    { "es-Latn-UY", "es-Latn-419" },
    { "es-Latn-VE", "es-Latn-419" },
    { "pt-Latn-PT", "pt-Latn-PT" },
    { "en-Latn-US", "en-Latn-US" }
};

set<std::string> LocaleConfig::validCaTag {
    "buddhist",
    "chinese",
    "coptic",
    "dangi",
    "ethioaa",
    "ethiopic",
    "gregory",
    "hebrew",
    "indian",
    "islamic",
    "islamic-umalqura",
    "islamic-tbla",
    "islamic-civil",
    "islamic-rgsa",
    "iso8601",
    "japanese",
    "persian",
    "roc",
    "islamicc",
};
set<std::string> LocaleConfig::validCoTag {
    "big5han",
    "compat",
    "dict",
    "direct",
    "ducet",
    "eor",
    "gb2312",
    "phonebk",
    "phonetic",
    "pinyin",
    "reformed",
    "searchjl",
    "stroke",
    "trad",
    "unihan",
    "zhuyin",
};
set<std::string> LocaleConfig::validKnTag {
    "true",
    "false",
};
set<std::string> LocaleConfig::validKfTag {
    "upper",
    "lower",
    "false",
};
set<std::string> LocaleConfig::validNuTag {
    "adlm", "ahom", "arab", "arabext", "bali", "beng",
    "bhks", "brah", "cakm", "cham", "deva", "diak",
    "fullwide", "gong", "gonm", "gujr", "guru", "hanidec",
    "hmng", "hmnp", "java", "kali", "khmr", "knda",
    "lana", "lanatham", "laoo", "latn", "lepc", "limb",
    "mathbold", "mathdbl", "mathmono", "mathsanb", "mathsans", "mlym",
    "modi", "mong", "mroo", "mtei", "mymr", "mymrshan",
    "mymrtlng", "newa", "nkoo", "olck", "orya", "osma",
    "rohg", "saur", "segment", "shrd", "sind", "sinh",
    "sora", "sund", "takr", "talu", "tamldec", "telu",
    "thai", "tibt", "tirh", "vaii", "wara", "wcho",
};
set<std::string> LocaleConfig::validHcTag {
    "h12",
    "h23",
    "h11",
    "h24",
};

static unordered_map<string, string> g_languageMap = {
    { "zh-Hans", "zh-Hans" },
    { "zh-Hant", "zh-Hant" },
    { "my-Qaag", "my-Qaag" },
    { "es-Latn-419", "es-419" },
    { "es-Latn-US", "es-419" },
    { "az-Latn", "az-Latn" },
    { "bs-Latn", "bs-Latn" },
    { "en-Qaag", "en-Qaag" },
    { "uz-Latn", "uz-Latn" },
    { "sr-Latn", "sr-Latn" },
    { "jv-Latn", "jv-Latn" },
    { "pt-Latn-BR", "pt-BR" },
    { "pa-Guru", "pa-Guru" },
    { "mai-Deva", "mai-Deva" }
};

string Adjust(const string &origin)
{
    for (auto iter = g_languageMap.begin(); iter != g_languageMap.end(); ++iter) {
        string key = iter->first;
        if (!origin.compare(0, key.length(), key)) {
            return iter->second;
        }
    }
    return origin;
}

int32_t GetDialectName(const char *localeName, char *name, size_t nameCapacity, UErrorCode &status)
{
    icu::Locale locale = icu::Locale::forLanguageTag(localeName, status);
    if (status != U_ZERO_ERROR) {
        return 0;
    }
    const char *lang = locale.getLanguage();
    const char *script = locale.getScript();
    const char *country = locale.getCountry();
    bool hasScript = (script != nullptr) && strlen(script) > 0;
    bool hasCountry = (country != nullptr) && strlen(country) > 0;
    string temp = lang;
    if (hasScript && hasCountry) {
        temp.append("_");
        temp.append(script);
        temp.append("_");
        temp.append(country);
    } else if (hasScript) {
        temp.append("_");
        temp.append(script);
    } else if (hasCountry) {
        temp.append("_");
        temp.append(country);
    }
    if (strcpy_s(name, nameCapacity, temp.data()) != EOK) {
        return 0;
    }
    return temp.size();
}

int32_t GetDisplayName(const char *locale, const char *displayLocale, UChar *dest, int32_t destCapacity,
    UErrorCode &status)
{
    if (status != U_ZERO_ERROR) {
        return 0;
    }
    if ((destCapacity < 0) || (destCapacity > 0  && !dest)) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        return 0;
    }
    char localeBuffer[ULOC_FULLNAME_CAPACITY];
    int32_t length = GetDialectName(locale, localeBuffer, sizeof(localeBuffer), status);
    if (status != U_ZERO_ERROR || !length) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        return 0;
    }
    const UChar *str = uloc_getTableStringWithFallback(U_ICUDATA_LANG, displayLocale, "Languages",
        nullptr, localeBuffer, &length, &status);
    if (status <= U_ZERO_ERROR) {
        int32_t len = (length < destCapacity) ? length : destCapacity;
        if ((len > 0) && (str != nullptr)) {
            u_memcpy(dest, str, len);
        }
    } else {
        status = U_USING_DEFAULT_WARNING;
        return 0;
    }
    return u_terminateUChars(dest, destCapacity, length, &status);
}

void GetDisplayLanguageImpl(const char *locale, const char *displayLocale, icu::UnicodeString &result)
{
    UChar *buffer = result.getBuffer(50);  // size 50 is enough to hold language name
    if (!buffer) {
        result.truncate(0);
        return;
    }
    UErrorCode status = U_ZERO_ERROR;
    int32_t length = GetDisplayName(locale, displayLocale, buffer, result.getCapacity(), status);
    result.releaseBuffer(U_SUCCESS(status) ? length : 0);
}

string GetDisplayLanguageInner(const string &language, const string &displayLocale, bool sentenceCase)
{
    icu::UnicodeString unistr;
    // 0 is the start position of language, 2 is the length of zh and fa
    if (!language.compare(0, 2, "zh") || !language.compare(0, 2, "fa")) {
        UErrorCode error = U_ZERO_ERROR;
        icu::Locale disLocale = icu::Locale::forLanguageTag(displayLocale, error);
        if (error != U_ZERO_ERROR) {
            return language;
        }
        const char *name = disLocale.getName();
        if (!name) {
            return language;
        }
        GetDisplayLanguageImpl(language.c_str(), name, unistr);
    } else {
        UErrorCode status = U_ZERO_ERROR;
        icu::Locale displayLoc = icu::Locale::forLanguageTag(displayLocale, status);
        if (status != U_ZERO_ERROR) {
            return "";
        }
        icu::Locale locale = icu::Locale::forLanguageTag(language, status);
        if (status != U_ZERO_ERROR) {
            return "";
        }
        locale.getDisplayName(displayLoc, unistr);
    }
    if (sentenceCase) {
        UChar32 ch = ucase_toupper(unistr.char32At(0));
        unistr.replace(0, 1, ch);
    }
    string out;
    unistr.toUTF8String(out);
    return out;
}

bool LocaleConfig::listsInitialized = LocaleConfig::InitializeLists();

string LocaleConfig::GetSystemLanguage()
{
    char value[CONFIG_LEN];
    int code = GetParameter(LANGUAGE_KEY, "", value, CONFIG_LEN);
    if (code > 0) {
        return value;
    }
    code = GetParameter(DEFAULT_LANGUAGE_KEY, "", value, CONFIG_LEN);
    if (code > 0) {
        return value;
    }
    return "";
}

string LocaleConfig::GetSystemRegion()
{
    string locale = GetSystemLocale();
    char value[CONFIG_LEN];
    int code = GetParameter(LOCALE_KEY, "", value, CONFIG_LEN);
    if (code > 0) {
        string tag(value, code);
        UErrorCode status = U_ZERO_ERROR;
        icu::Locale origin = icu::Locale::forLanguageTag(tag, status);
        if (status == U_ZERO_ERROR) {
            const char *country = origin.getCountry();
            if (country != nullptr) {
                return country;
            }
        }
    }
    code = GetParameter(DEFAULT_REGION_KEY, "", value, CONFIG_LEN);
    if (code > 0) {
        return value;
    }
    return "";
}

string LocaleConfig::GetSystemLocale()
{
    char value[CONFIG_LEN];
    int code = GetParameter(LOCALE_KEY, "", value, CONFIG_LEN);
    if (code > 0) {
        return value;
    }
    code = GetParameter(DEFAULT_LOCALE_KEY, "", value, CONFIG_LEN);
    if (code > 0) {
        return value;
    }
    return "";
}

bool LocaleConfig::CheckPermission()
{
    Security::AccessToken::AccessTokenID callerToken = IPCSkeleton::GetCallingTokenID();
    int result = Security::AccessToken::PermissionState::PERMISSION_GRANTED;
    if (Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(callerToken)
        == Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE) {
        result = Security::AccessToken::AccessTokenKit::VerifyNativeToken(callerToken,
                                                                          "ohos.permission.UPDATE_CONFIGURATION");
    } else if (Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(callerToken)
        == Security::AccessToken::ATokenTypeEnum::TOKEN_HAP) {
        result = Security::AccessToken::AccessTokenKit::VerifyAccessToken(callerToken,
                                                                          "ohos.permission.UPDATE_CONFIGURATION");
    } else {
        HiLog::Info(LABEL, "Invlid tokenID");
        return false;
    }
    if (result != Security::AccessToken::PermissionState::PERMISSION_GRANTED) {
        HiLog::Info(LABEL, "Verify permission failed");
        return false;
    }
    return true;
}

bool LocaleConfig::SetSystemLanguage(const string &language)
{
    if (!CheckPermission()) {
        return false;
    }
    if (!IsValidTag(language)) {
        return false;
    }
    return SetParameter(LANGUAGE_KEY, language.data()) == 0;
}

bool LocaleConfig::SetSystemRegion(const string &region)
{
    if (!CheckPermission()) {
        return false;
    }
    if (!IsValidRegion(region)) {
        return false;
    }
    char value[CONFIG_LEN];
    int code = GetParameter(LOCALE_KEY, "", value, CONFIG_LEN);
    string newLocale;
    if (code > 0) {
        string tag(value, code);
        newLocale = GetRegionChangeLocale(tag, region);
        if (newLocale == "") {
            return false;
        }
    } else {
        icu::Locale temp("", region.c_str());
        UErrorCode status = U_ZERO_ERROR;
        temp.addLikelySubtags(status);
        if (status != U_ZERO_ERROR) {
            return false;
        }
        newLocale = temp.toLanguageTag<string>(status);
        if (status != U_ZERO_ERROR) {
            return false;
        }
    }
    return SetParameter(LOCALE_KEY, newLocale.data()) == 0;
}

bool LocaleConfig::SetSystemLocale(const string &locale)
{
    if (!CheckPermission()) {
        return false;
    }
    if (!IsValidTag(locale)) {
        return false;
    }
    return SetParameter(LOCALE_KEY, locale.data()) == 0;
}

bool LocaleConfig::IsValidLanguage(const string &language)
{
    string::size_type size = language.size();
    if ((size != LANGUAGE_LEN) && (size != LANGUAGE_LEN + 1)) {
        return false;
    }
    for (size_t i = 0; i < size; ++i) {
        if ((language[i] > 'z') || (language[i] < 'a')) {
            return false;
        }
    }
    return true;
}

bool LocaleConfig::IsValidScript(const string &script)
{
    string::size_type size = script.size();
    if (size != LocaleInfo::SCRIPT_LEN) {
        return false;
    }
    char first = script[0];
    if ((first < 'A') || (first > 'Z')) {
        return false;
    }
    for (string::size_type i = 1; i < LocaleInfo::SCRIPT_LEN; ++i) {
        if ((script[i] > 'z') || (script[i] < 'a')) {
            return false;
        }
    }
    return true;
}

bool LocaleConfig::IsValidRegion(const string &region)
{
    string::size_type size = region.size();
    if (size != LocaleInfo::REGION_LEN) {
        return false;
    }
    for (size_t i = 0; i < LocaleInfo::REGION_LEN; ++i) {
        if ((region[i] > 'Z') || (region[i] < 'A')) {
            return false;
        }
    }
    return true;
}

bool LocaleConfig::IsValidTag(const string &tag)
{
    if (!tag.size()) {
        return false;
    }
    vector<string> splits;
    Split(tag, "-", splits);
    if (!IsValidLanguage(splits[0])) {
        return false;
    }
    return true;
}

void LocaleConfig::Split(const string &src, const string &sep, vector<string> &dest)
{
    string::size_type begin = 0;
    string::size_type end = src.find(sep);
    while (end != string::npos) {
        dest.push_back(src.substr(begin, end - begin));
        begin = end + sep.size();
        end = src.find(sep, begin);
    }
    if (begin != src.size()) {
        dest.push_back(src.substr(begin));
    }
}

// language in white languages should have script.
void LocaleConfig::GetSystemLanguages(vector<string> &ret)
{
    for (auto item : whiteLanguages) {
        ret.push_back(item);
    }
}

const unordered_set<string>& LocaleConfig::GetSupportedLocales()
{
    return supportedLocales;
}

const unordered_set<string>& LocaleConfig::GetSupportedRegions()
{
    return supportedRegions;
}

void LocaleConfig::GetSystemCountries(vector<string> &ret)
{
    for (auto item : supportedRegions) {
        ret.push_back(item);
    }
}

bool LocaleConfig::IsSuggested(const string &language)
{
    unordered_set<string> relatedLocales;
    vector<string> simCountries;
    GetCountriesFromSim(simCountries);
    GetRelatedLocales(relatedLocales, simCountries);
    for (auto iter = relatedLocales.begin(); iter != relatedLocales.end();) {
        if (whiteLanguages.find(*iter) == whiteLanguages.end()) {
            iter = relatedLocales.erase(iter);
        } else {
            ++iter;
        }
    }
    string mainLanguage = GetMainLanguage(language);
    return relatedLocales.find(mainLanguage) != relatedLocales.end();
}

bool LocaleConfig::IsSuggested(const std::string &language, const std::string &region)
{
    unordered_set<string> relatedLocales;
    vector<string> countries { region };
    GetRelatedLocales(relatedLocales, countries);
    for (auto iter = relatedLocales.begin(); iter != relatedLocales.end();) {
        if (whiteLanguages.find(*iter) == whiteLanguages.end()) {
            iter = relatedLocales.erase(iter);
        } else {
            ++iter;
        }
    }
    string mainLanguage = GetMainLanguage(language);
    return relatedLocales.find(mainLanguage) != relatedLocales.end();
}

void LocaleConfig::GetRelatedLocales(unordered_set<string> &relatedLocales, vector<string> countries)
{
    // remove unsupported countries
    const unordered_set<string> &regions = GetSupportedRegions();
    for (auto iter = countries.begin(); iter != countries.end();) {
        if (regions.find(*iter) == regions.end()) {
            iter = countries.erase(iter);
        } else {
            ++iter;
        }
    }
    const unordered_set<string> &locales = GetSupportedLocales();
    for (string locale : locales) {
        bool find = false;
        for (string country : countries) {
            if (locale.find(country) != string::npos) {
                find = true;
                break;
            }
        }
        if (!find) {
            continue;
        }
        string mainLanguage = GetMainLanguage(locale);
        if (mainLanguage != "") {
            relatedLocales.insert(mainLanguage);
        }
    }
}

void LocaleConfig::GetCountriesFromSim(vector<string> &simCountries)
{
    simCountries.push_back(GetSystemRegion());
#ifdef TEL_CORE_SERVICE_EXISTS
    simCountries.push_back(Str16ToStr8(
        DelayedRefSingleton<Telephony::CoreServiceClient>::GetInstance().GetISOCountryCodeForSim(0)));
#endif
}

void LocaleConfig::GetListFromFile(const char *path, const char *resourceName, unordered_set<string> &ret)
{
    xmlKeepBlanksDefault(0);
    if (!path) {
        return;
    }
    xmlDocPtr doc = xmlParseFile(path);
    if (!doc) {
        return;
    }
    xmlNodePtr cur = xmlDocGetRootElement(doc);
    if (!cur || xmlStrcmp(cur->name, reinterpret_cast<const xmlChar *>(resourceName))) {
        xmlFreeDoc(doc);
        return;
    }
    cur = cur->xmlChildrenNode;
    xmlChar *content = nullptr;
    while (cur != nullptr) {
        content = xmlNodeGetContent(cur);
        if (content != nullptr) {
            ret.insert(reinterpret_cast<const char*>(content));
            xmlFree(content);
            cur = cur->next;
        } else {
            break;
        }
    }
    xmlFreeDoc(doc);
}

void LocaleConfig::Expunge(unordered_set<string> &src, const unordered_set<string> &another)
{
    for (auto iter = src.begin(); iter != src.end();) {
        if (another.find(*iter) != another.end()) {
            iter = src.erase(iter);
        } else {
            ++iter;
        }
    }
}

bool LocaleConfig::InitializeLists()
{
    SetHwIcuDirectory();
    GetListFromFile(SUPPORTED_REGIONS_PATH, SUPPORTED_REGIONS_NAME, supportedRegions);
    unordered_set<string> forbiddenRegions;
    GetListFromFile(FORBIDDEN_REGIONS_PATH, FORBIDDEN_REGIONS_NAME, forbiddenRegions);
    Expunge(supportedRegions, forbiddenRegions);
    GetListFromFile(WHITE_LANGUAGES_PATH, WHITE_LANGUAGES_NAME, whiteLanguages);
    unordered_set<string> forbiddenLanguages;
    GetListFromFile(FORBIDDEN_LANGUAGES_PATH, FORBIDDEN_LANGUAGES_NAME, forbiddenLanguages);
    Expunge(whiteLanguages, forbiddenLanguages);
    GetListFromFile(SUPPORTED_LOCALES_PATH, SUPPORTED_LOCALES_NAME, supportedLocales);
    return true;
}

string LocaleConfig::GetRegionChangeLocale(const string &languageTag, const string &region)
{
    UErrorCode status = U_ZERO_ERROR;
    const icu::Locale origin = icu::Locale::forLanguageTag(languageTag, status);
    if (status != U_ZERO_ERROR) {
        return "";
    }
    icu::LocaleBuilder builder = icu::LocaleBuilder().setLanguage(origin.getLanguage()).
        setScript(origin.getScript()).setRegion(region);
    icu::Locale temp = builder.setExtension('u', "").build(status);
    if (status != U_ZERO_ERROR) {
        return "";
    }
    string ret = temp.toLanguageTag<string>(status);
    return (status != U_ZERO_ERROR) ? "" : ret;
}

string LocaleConfig::GetMainLanguage(const string &language)
{
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale origin = icu::Locale::forLanguageTag(language, status);
    if (status != U_ZERO_ERROR) {
        return "";
    }
    origin.addLikelySubtags(status);
    if (status != U_ZERO_ERROR) {
        return "";
    }
    icu::LocaleBuilder builder = icu::LocaleBuilder().setLanguage(origin.getLanguage()).
        setScript(origin.getScript()).setRegion(origin.getCountry());
    icu::Locale temp = builder.setExtension('u', "").build(status);
    string fullLanguage = temp.toLanguageTag<string>(status);
    if (status != U_ZERO_ERROR) {
        return "";
    }
    if (dialectMap.find(fullLanguage) != dialectMap.end()) {
        return dialectMap[fullLanguage];
    }
    builder.setRegion("");
    temp = builder.build(status);
    fullLanguage = temp.toLanguageTag<string>(status);
    if (status != U_ZERO_ERROR) {
        return "";
    }
    return fullLanguage;
}

string LocaleConfig::GetDisplayLanguage(const string &language, const string &displayLocale, bool sentenceCase)
{
    string adjust = Adjust(language);
    if (adjust == language) {
        UErrorCode status = U_ZERO_ERROR;
        icu::Locale displayLoc = icu::Locale::forLanguageTag(displayLocale, status);
        if (status != U_ZERO_ERROR) {
            return "";
        }
        icu::Locale locale = icu::Locale::forLanguageTag(language, status);
        if (status != U_ZERO_ERROR) {
            return "";
        }
        icu::UnicodeString unistr;
        locale.getDisplayLanguage(displayLoc, unistr);
        if (sentenceCase) {
            UChar32 ch = ucase_toupper(unistr.char32At(0));
            unistr.replace(0, 1, ch);
        }
        string out;
        unistr.toUTF8String(out);
        return out;
    }
    return GetDisplayLanguageInner(adjust, displayLocale, sentenceCase);
}

string LocaleConfig::GetDisplayRegion(const string &region, const string &displayLocale, bool sentenceCase)
{
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale originLocale;
    if (IsValidRegion(region)) {
        icu::LocaleBuilder builder = icu::LocaleBuilder().setRegion(region);
        originLocale = builder.build(status);
    } else {
        originLocale = icu::Locale::forLanguageTag(region, status);
        originLocale.addLikelySubtags(status);
    }
    if (status != U_ZERO_ERROR) {
        return "";
    }
    icu::Locale locale = icu::Locale::forLanguageTag(displayLocale, status);
    if (status != U_ZERO_ERROR) {
        return "";
    }
    icu::UnicodeString displayRegion;
    originLocale.getDisplayCountry(locale, displayRegion);
    if (sentenceCase) {
        UChar32 ch = ucase_toupper(displayRegion.char32At(0));
        displayRegion.replace(0, 1, ch);
    }
    string temp;
    displayRegion.toUTF8String(temp);
    return temp;
}

bool LocaleConfig::IsRTL(const string &locale)
{
    icu::Locale curLocale(locale.c_str());
    return curLocale.isRightToLeft();
}

std::string ContainTag(std::string &localeTag, std::string &defaultLocaleTag,
    const std::string &extensionTag)
{
    std::string tag = localeTag;
    std::size_t found = tag.find(extensionTag);
    if (found == std::string::npos) {
        tag = defaultLocaleTag;
        found = tag.find(extensionTag);
    }
    if (found == std::string::npos) {
        return "";
    }

    std::size_t start = found + 4;  // 4 is the tag length
    std::size_t end = tag.find("-", start);

    return tag.substr(start, end - start);
}

void parseExtension(const std::string &extension, std::map<std::string, std::string> &map)
{
    std::string pattern = "-..-";
    std::regex express(pattern);

    std::regex_token_iterator<std::string::const_iterator> begin1(extension.cbegin(), extension.cend(), express);
    std::regex_token_iterator<std::string::const_iterator> begin2(extension.cbegin(), extension.cend(), express, -1);
    begin2++;
    for (; begin1 != std::sregex_token_iterator() && begin2 != std::sregex_token_iterator(); begin1++, begin2++) {
        map.insert(std::pair<std::string, std::string>(begin1->str(), begin2->str()));
    }
}

void setExtension(std::string &extension, const std::string &tag, const std::set<string> &validValue,
    const std::map<std::string, std::string> &extensionMap,
    const std::map<std::string, std::string> &defaultExtensionMap)
{
    std::string value = "";
    auto it = extensionMap.find(tag);
    if (it != extensionMap.end()) {
        value = it->second;
        if (validValue.find(value) == validValue.end()) {
            return;
        } else {
            extension += tag;
            extension += value;
        }
    } else {
        it = defaultExtensionMap.find(tag);
        if (it != defaultExtensionMap.end()) {
            value = it->second;
            if (validValue.find(value) == validValue.end()) {
                return;
            } else {
                extension += tag;
                extension += value;
            }
        }
    }
}

void setOtherExtension(std::string &extension, std::map<std::string, std::string> &extensionMap,
    std::map<std::string, std::string> &defaultExtensionMap)
{
    std::set<std::string> tags;
    tags.insert("-ca-");
    tags.insert("-co-");
    tags.insert("-kn-");
    tags.insert("-kf-");
    tags.insert("-nu-");
    tags.insert("-hc-");

    for (auto it = tags.begin(); it != tags.end(); it++) {
        extensionMap.erase(*it);
        defaultExtensionMap.erase(*it);
    }

    for (auto it = defaultExtensionMap.begin(); it != defaultExtensionMap.end(); it++) {
        extensionMap.insert(std::pair<std::string, std::string>(it->first, it->second));
    }

    for (auto it = extensionMap.begin(); it != extensionMap.end(); it++) {
        extension += it->first;
        extension += it->second;
    }
}

std::string LocaleConfig::GetValidLocale(const std::string &localeTag)
{
    std::string baseLocale = "";
    std::string extension = "";
    std::size_t found = localeTag.find("-u-");
    baseLocale = localeTag.substr(0, found);
    if (found != std::string::npos) {
        extension = localeTag.substr(found);
    }
    std::map<std::string, std::string> extensionMap;
    if (extension != "") {
        parseExtension(extension, extensionMap);
    }

    std::string systemLocaleTag = GetSystemLocale();
    std::string defaultExtension = "";
    found = systemLocaleTag.find("-u-");
    if (found != std::string::npos) {
        defaultExtension = systemLocaleTag.substr(found);
    }
    std::map<std::string, std::string> defaultExtensionMap;
    if (defaultExtension != "") {
        parseExtension(defaultExtension, defaultExtensionMap);
    }

    std::string ext = "";
    setExtension(ext, "-ca-", validCaTag, extensionMap, defaultExtensionMap);
    setExtension(ext, "-co-", validCoTag, extensionMap, defaultExtensionMap);
    setExtension(ext, "-kn-", validKnTag, extensionMap, defaultExtensionMap);
    setExtension(ext, "-kf-", validKfTag, extensionMap, defaultExtensionMap);
    setExtension(ext, "-nu-", validNuTag, extensionMap, defaultExtensionMap);
    setExtension(ext, "-hc-", validHcTag, extensionMap, defaultExtensionMap);

    std::string otherExt = "";
    setOtherExtension(otherExt, extensionMap, defaultExtensionMap);
    if (ext != "" || otherExt != "") {
        return baseLocale + "-u" + ext + otherExt;
    } else {
        return baseLocale;
    }
}

bool LocaleConfig::Is24HourClock()
{
    char value[CONFIG_LEN];
    int code = GetParameter(HOUR_KEY, "", value, CONFIG_LEN);
    if (code <= 0) {
        return false;
    }
    if (!strcmp(value, "true")) {
        return true;
    }
    return false;
}

bool LocaleConfig::Set24HourClock(bool option)
{
    if (!CheckPermission()) {
        return false;
    }
    std::string optionStr = "";
    if (option) {
        optionStr = "true";
    } else {
        optionStr = "false";
    }
    return SetParameter(HOUR_KEY, optionStr.data()) == 0;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS
