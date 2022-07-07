/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "locale_config.h"
#include "unicode/locid.h"
#include "unicode/unistr.h"
#include "i18n_timezone.h"

namespace OHOS {
namespace Global {
namespace I18n {
I18nTimeZone::I18nTimeZone(std::string zoneID)
{
    if (zoneID.empty()) {
        timezone = icu::TimeZone::createDefault();
    } else {
        icu::UnicodeString unicodeZoneID(zoneID.data(), zoneID.length());
        timezone = icu::TimeZone::createTimeZone(unicodeZoneID);
    }
}

I18nTimeZone::~I18nTimeZone()
{
    if (timezone != nullptr) {
        delete timezone;
        timezone = nullptr;
    }
}

icu::TimeZone* I18nTimeZone::GetTimeZone()
{
    return timezone;
}

std::unique_ptr<I18nTimeZone> I18nTimeZone::CreateInstance(std::string zoneID)
{
    std::unique_ptr<I18nTimeZone> i18nTimeZone = std::make_unique<I18nTimeZone>(zoneID);
    if (i18nTimeZone->GetTimeZone() == nullptr) {
        return nullptr;
    }
    return i18nTimeZone;
}

int32_t I18nTimeZone::GetOffset(double date)
{
    int32_t rawOffset = 0;
    int32_t dstOffset = 0;
    bool local = false;
    UErrorCode status = U_ZERO_ERROR;
    timezone->getOffset(date, (UBool)local, rawOffset, dstOffset, status);
    if (status != U_ZERO_ERROR) {
        return 0;
    }
    return rawOffset + dstOffset;
}

int32_t I18nTimeZone::GetRawOffset()
{
    return timezone->getRawOffset();
}

std::string I18nTimeZone::GetID()
{
    icu::UnicodeString zoneID;
    timezone->getID(zoneID);
    std::string result;
    zoneID.toUTF8String(result);
    return result;
}

std::string I18nTimeZone::GetDisplayName()
{
    std::string localeStr = LocaleConfig::GetSystemLocale();
    return GetDisplayName(localeStr, false);
}

std::string I18nTimeZone::GetDisplayName(bool isDST)
{
    std::string localeStr = LocaleConfig::GetSystemLocale();
    return GetDisplayName(localeStr, isDST);
}

std::string I18nTimeZone::GetDisplayName(std::string localeStr)
{
    return GetDisplayName(localeStr, false);
}

std::string I18nTimeZone::GetDisplayName(std::string localeStr, bool isDST)
{
    icu::TimeZone::EDisplayType style = icu::TimeZone::EDisplayType::LONG_GENERIC;
    icu::Locale locale(localeStr.data());
    icu::UnicodeString name;
    timezone->getDisplayName((UBool)isDST, style, locale, name);
    std::string result;
    name.toUTF8String(result);
    return result;
}
}
}
}