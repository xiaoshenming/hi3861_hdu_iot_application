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
#include "collator.h"

#include <stringpiece.h>

#include "unicode/ucol.h"
#include "unicode/errorcode.h"
#include "unicode/uloc.h"
#include "locale_config.h"

namespace OHOS {
namespace Global {
namespace I18n {
std::string Collator::ParseOption(std::map<std::string, std::string> &options, const std::string &key)
{
    std::map<std::string, std::string>::iterator it = options.find(key);
    if (it != options.end()) {
        return it->second;
    } else {
        return "";
    }
}

void Collator::ParseAllOptions(std::map<std::string, std::string> &options)
{
    localeMatcher = ParseOption(options, "localeMatcher");
    if (localeMatcher == "") {
        localeMatcher = "best fit";
    }

    usage = ParseOption(options, "usage");
    if (usage == "") {
        usage = "sort";
    }

    sensitivity = ParseOption(options, "sensitivity");
    if (sensitivity == "") {
        sensitivity = "variant";
    }

    ignorePunctuation = ParseOption(options, "ignorePunctuation");
    if (ignorePunctuation == "") {
        ignorePunctuation = "false";
    }

    numeric = ParseOption(options, "numeric");
    caseFirst = ParseOption(options, "caseFirst");
    collation = ParseOption(options, "collation");
}

Collator::Collator(std::vector<std::string> &localeTags, std::map<std::string, std::string> &options)
{
    ParseAllOptions(options);

    UErrorCode status = UErrorCode::U_ZERO_ERROR;
    if (!localeTags.size()) {
        localeTags.push_back(LocaleConfig::GetSystemLocale());
    }
    for (size_t i = 0; i < localeTags.size(); i++) {
        std::string curLocale = localeTags[i];
        locale = icu::Locale::forLanguageTag(icu::StringPiece(curLocale), status);
        if (LocaleInfo::allValidLocales.count(locale.getLanguage()) > 0) {
            localeInfo = std::make_unique<LocaleInfo>(curLocale, options);
            locale = localeInfo->GetLocale();
            localeStr = localeInfo->GetBaseName();
            bool createSuccess = InitCollator();
            if (!createSuccess) {
                continue;
            }
            break;
        }
    }
}

bool Collator::IsValidCollation(std::string &collation, UErrorCode &status)
{
    const char *currentCollation = uloc_toLegacyType("collation", collation.c_str());
    if (currentCollation != nullptr) {
        std::unique_ptr<icu::StringEnumeration> enumeration(
            icu::Collator::getKeywordValuesForLocale("collation", icu::Locale(locale.getBaseName()), false, status));
        int length;
        const char *validCollations = nullptr;
        if (enumeration != nullptr) {
            validCollations = enumeration->next(&length, status);
        }
        while (validCollations != nullptr) {
            if (!strcmp(validCollations, currentCollation)) {
                return true;
            }
            if (enumeration != nullptr) {
                validCollations = enumeration->next(&length, status);
            }
        }
    }
    return false;
}

void Collator::SetCollation(UErrorCode &status)
{
    if (collation != "") {
        if (IsValidCollation(collation, status)) {
            locale.setUnicodeKeywordValue("co", collation, status);
        } else {
            collation = "default";
            locale.setUnicodeKeywordValue("co", nullptr, status);
        }
    } else {
        collation = localeInfo->GetCollation();
        if (collation != "") {
            if (IsValidCollation(collation, status)) {
                locale.setUnicodeKeywordValue("co", collation, status);
            } else {
                locale.setUnicodeKeywordValue("co", nullptr, status);
                collation = "default";
            }
        } else {
            locale.setUnicodeKeywordValue("co", nullptr, status);
            collation = "default";
        }
    }
}

void Collator::SetUsage(UErrorCode &status)
{
    if (usage == "search") {
        collation = "default";
        locale.setUnicodeKeywordValue("co", nullptr, status);
    }
}

void Collator::SetNumeric(UErrorCode &status)
{
    if (!collatorPtr) {
        return;
    }
    if (numeric == "") {
        numeric = localeInfo->GetNumeric();
        if (numeric != "true" && numeric != "false") {
            numeric = "false";
        }
    }
    if (numeric == "true") {
        collatorPtr->setAttribute(UColAttribute::UCOL_NUMERIC_COLLATION,
            UColAttributeValue::UCOL_ON, status);
    } else {
        collatorPtr->setAttribute(UColAttribute::UCOL_NUMERIC_COLLATION,
            UColAttributeValue::UCOL_OFF, status);
    }
}

void Collator::SetCaseFirst(UErrorCode &status)
{
    if (!collatorPtr) {
        return;
    }
    if (caseFirst == "") {
        caseFirst = localeInfo->GetCaseFirst();
        if (caseFirst != "upper" && caseFirst != "lower" && caseFirst != "false") {
            caseFirst = "false";
        }
    }
    if (caseFirst == "upper") {
        collatorPtr->setAttribute(UColAttribute::UCOL_CASE_FIRST,
            UColAttributeValue::UCOL_UPPER_FIRST, status);
    } else if (caseFirst == "lower") {
        collatorPtr->setAttribute(UColAttribute::UCOL_CASE_FIRST,
            UColAttributeValue::UCOL_LOWER_FIRST, status);
    } else {
        collatorPtr->setAttribute(UColAttribute::UCOL_CASE_FIRST,
            UColAttributeValue::UCOL_OFF, status);
    }
}

void Collator::SetSensitivity(UErrorCode &status)
{
    if (!collatorPtr) {
        return;
    }
    if (sensitivity == "base") {
        collatorPtr->setStrength(icu::Collator::PRIMARY);
    } else if (sensitivity == "accent") {
        collatorPtr->setStrength(icu::Collator::SECONDARY);
    } else if (sensitivity == "case") {
        collatorPtr->setStrength(icu::Collator::PRIMARY);
        collatorPtr->setAttribute(UColAttribute::UCOL_CASE_LEVEL,
            UColAttributeValue::UCOL_ON, status);
    } else {
        collatorPtr->setStrength(icu::Collator::TERTIARY);
    }
}

void Collator::SetIgnorePunctuation(UErrorCode &status)
{
    if (!collatorPtr) {
        return;
    }
    if (ignorePunctuation == "true") {
        collatorPtr->setAttribute(UColAttribute::UCOL_ALTERNATE_HANDLING,
            UColAttributeValue::UCOL_SHIFTED, status);
    }
}

bool Collator::InitCollator()
{
    UErrorCode status = UErrorCode::U_ZERO_ERROR;
    SetCollation(status);
    SetUsage(status);
    collatorPtr = icu::Collator::createInstance(locale, status);
    SetNumeric(status);
    SetCaseFirst(status);
    SetSensitivity(status);
    SetIgnorePunctuation(status);

    if (!collatorPtr) {
        return false;
    }
    return true;
}

Collator::~Collator()
{
    if (collatorPtr != nullptr) {
        delete collatorPtr;
        collatorPtr = nullptr;
    }
}

CompareResult Collator::Compare(const std::string &first, const std::string &second)
{
    if (!collatorPtr) {
        return CompareResult::INVALID;
    }
    icu::Collator::EComparisonResult result = collatorPtr->compare(icu::UnicodeString(first.data(), first.length()),
        icu::UnicodeString(second.data(), second.length()));
    if (result == icu::Collator::EComparisonResult::LESS) {
        return CompareResult::SMALLER;
    } else if (result == icu::Collator::EComparisonResult::EQUAL) {
        return CompareResult::EQUAL;
    } else {
        return CompareResult::GREATER;
    }
}

void Collator::ResolvedOptions(std::map<std::string, std::string> &options)
{
    options.insert(std::pair<std::string, std::string>("localeMatcher", localeMatcher));
    options.insert(std::pair<std::string, std::string>("locale", localeStr));
    options.insert(std::pair<std::string, std::string>("usage", usage));
    options.insert(std::pair<std::string, std::string>("sensitivity", sensitivity));
    options.insert(std::pair<std::string, std::string>("ignorePunctuation", ignorePunctuation));
    options.insert(std::pair<std::string, std::string>("numeric", numeric));
    options.insert(std::pair<std::string, std::string>("caseFirst", caseFirst));
    options.insert(std::pair<std::string, std::string>("collation", collation));
}
} // namespace I18n
} // namespace Global
} // namespace OHOS
