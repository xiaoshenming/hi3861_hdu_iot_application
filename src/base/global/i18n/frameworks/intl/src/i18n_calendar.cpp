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

#include "i18n_calendar.h"
#include "unicode/gregocal.h"
#include "buddhcal.h"
#include "chnsecal.h"
#include "coptccal.h"
#include "ethpccal.h"
#include "hebrwcal.h"
#include "indiancal.h"
#include "islamcal.h"
#include "japancal.h"
#include "persncal.h"
#include "ureslocs.h"
#include "ulocimp.h"
#include "uresimp.h"
#include "ustring.h"
#include "ustr_imp.h"
#include "hilog/log.h"

namespace OHOS {
namespace Global {
namespace I18n {
I18nCalendar::I18nCalendar(std::string localeTag)
{
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale tempLocale = icu::Locale::forLanguageTag(localeTag, status);
    if (status != U_ZERO_ERROR) {
        calendar_ = new icu::GregorianCalendar(status);
        return;
    }
    calendar_ = icu::Calendar::createInstance(tempLocale, status);
}

I18nCalendar::I18nCalendar(std::string localeTag, CalendarType type)
{
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale tempLocale = icu::Locale::forLanguageTag(localeTag, status);
    if (status != U_ZERO_ERROR) {
        calendar_ = new icu::GregorianCalendar(status);
        return;
    }
    InitCalendar(tempLocale, type);
}

void I18nCalendar::InitCalendar(const icu::Locale &locale, CalendarType type)
{
    UErrorCode status = U_ZERO_ERROR;
    switch (type) {
        case BUDDHIST: {
            calendar_ = new icu::BuddhistCalendar(locale, status);
            break;
        }
        case CHINESE: {
            calendar_ = new icu::ChineseCalendar(locale, status);
            break;
        }
        case COPTIC: {
            calendar_ = new icu::CopticCalendar(locale, status);
            break;
        }
        case ETHIOPIC: {
            calendar_ = new icu::EthiopicCalendar(locale, status);
            break;
        }
        case HEBREW: {
            calendar_ = new icu::HebrewCalendar(locale, status);
            break;
        }
        case INDIAN: {
            calendar_ = new icu::IndianCalendar(locale, status);
            break;
        }
        case ISLAMIC_CIVIL: {
            calendar_ = new icu::IslamicCalendar(locale, status, icu::IslamicCalendar::ECalculationType::CIVIL);
            break;
        }
        default: {
            InitCalendar2(locale, type);
        }
    }
}

void I18nCalendar::InitCalendar2(const icu::Locale &locale, CalendarType type)
{
    UErrorCode status = U_ZERO_ERROR;
    switch (type) {
        case ISLAMIC_TBLA: {
            calendar_ = new icu::IslamicCalendar(locale, status, icu::IslamicCalendar::ECalculationType::TBLA);
            break;
        }
        case ISLAMIC_UMALQURA: {
            calendar_ = new icu::IslamicCalendar(locale, status, icu::IslamicCalendar::ECalculationType::UMALQURA);
            break;
        }
        case JAPANESE: {
            calendar_ = new icu::JapaneseCalendar(locale, status);
            break;
        }
        case PERSIAN: {
            calendar_ = new icu::PersianCalendar(locale, status);
            break;
        }
        case GREGORY: {
            calendar_ = new icu::GregorianCalendar(locale, status);
            break;
        }
        default: {
            calendar_ = icu::Calendar::createInstance(locale, status);
        }
    }
}

I18nCalendar::~I18nCalendar()
{
    delete calendar_;
}

void I18nCalendar::SetTime(double value)
{
    if (calendar_ != nullptr) {
        UErrorCode status = U_ZERO_ERROR;
        calendar_->setTime(value, status);
        return;
    }
}

void I18nCalendar::SetTimeZone(std::string id)
{
    icu::UnicodeString zone = icu::UnicodeString::fromUTF8(id);
    icu::TimeZone *timezone = icu::TimeZone::createTimeZone(zone);
    if (!timezone) {
        return;
    }
    if (calendar_ != nullptr) {
        calendar_->setTimeZone(*timezone);
    }
    delete(timezone);
}

std::string I18nCalendar::GetTimeZone(void)
{
    std::string ret;
    if (calendar_) {
        icu::UnicodeString unistr;
        calendar_->getTimeZone().getDisplayName(unistr);
        unistr.toUTF8String<std::string>(ret);
    }
    return ret;
}

void I18nCalendar::Set(int32_t year, int32_t month, int32_t date)
{
    if (calendar_ != nullptr) {
        calendar_->set(year, month, date);
        return;
    }
}

void I18nCalendar::Set(UCalendarDateFields field, int32_t value)
{
    if (calendar_ != nullptr) {
        calendar_->set(field, value);
        return;
    }
}

int32_t I18nCalendar::Get(UCalendarDateFields field) const
{
    if (calendar_ != nullptr) {
        UErrorCode status = U_ZERO_ERROR;
        return calendar_->get(field, status);
    }
    return 0;
}

void I18nCalendar::SetMinimalDaysInFirstWeek(int32_t value)
{
    if (calendar_ != nullptr) {
        calendar_->setMinimalDaysInFirstWeek((uint8_t)value);
        return;
    }
}

void I18nCalendar::SetFirstDayOfWeek(int32_t value)
{
    if (value < UCalendarDaysOfWeek::UCAL_SUNDAY || value > UCAL_SATURDAY) {
        return;
    }
    if (calendar_ != nullptr) {
        calendar_->setFirstDayOfWeek(UCalendarDaysOfWeek(value));
        return;
    }
}

int32_t I18nCalendar::GetMinimalDaysInFirstWeek(void)
{
    if (calendar_ != nullptr) {
        return calendar_->getMinimalDaysInFirstWeek();
    }
    return 1;
}

int32_t I18nCalendar::GetFirstDayOfWeek(void)
{
    if (calendar_ != nullptr) {
        return static_cast<int>(calendar_->getFirstDayOfWeek());
    }
    return UCAL_SUNDAY;
}

bool I18nCalendar::IsWeekend(int64_t date, UErrorCode &status)
{
    if (calendar_ != nullptr) {
        return calendar_->isWeekend(date, status);
    }
    return false;
}

bool I18nCalendar::IsWeekend(void)
{
    if (calendar_ != nullptr) {
        return calendar_->isWeekend();
    }
    return false;
}

std::string I18nCalendar::GetDisplayName(std::string &displayLocale)
{
    icu::UnicodeString unistr;
    int32_t destCapacity = 50;
    UChar *buffer = unistr.getBuffer(destCapacity);
    if (buffer == 0 || !calendar_) {
        return "";
    }
    const char *type = calendar_->getType();
    if (!type) {
        return "";
    }
    int32_t length;
    UErrorCode status = U_ZERO_ERROR;
    const UChar *str = uloc_getTableStringWithFallback(U_ICUDATA_LANG, displayLocale.c_str(), "Types", "calendar",
        type, &length, &status);
    int32_t len;
    if (status == U_ZERO_ERROR) {
        len = (length < destCapacity) ? length : destCapacity;
        if ((len > 0) && (str != nullptr)) {
            u_memcpy(buffer, str, len);
        }
    } else {
        return "";
    }
    len = u_terminateUChars(buffer, destCapacity, length, &status);
    unistr.releaseBuffer(U_SUCCESS(status) ? len : 0);
    std::string ret;
    unistr.toUTF8String<std::string>(ret);
    return ret;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS