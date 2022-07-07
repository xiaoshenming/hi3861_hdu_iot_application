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

#ifndef HAP_RESTORECON_H
#define HAP_RESTORECON_H

#include <selinux/context.h>
#include <string>
#include <unordered_map>
#include <vector>

#define SELINUX_HAP_RESTORECON_RECURSE 1

// parameters of each SehapInfo in file sehap_contexts
struct SehapInfo {
    std::string apl = "";
    std::string name = "";
    std::string domain = "";
    std::string type = "";
};

class HapContext {
public:
    HapContext();
    ~HapContext();
    int HapFileRestorecon(std::vector<std::string> &pathNameOrig, const std::string &apl,
                          const std::string &packageName, unsigned int flags);
    int HapFileRestorecon(const std::string &pathNameOrig, const std::string &apl, const std::string &packageName,
                          unsigned int flags);
    int HapDomainSetcontext(const std::string &apl, const std::string &packageName);

protected:
    static std::unordered_map<std::string, struct SehapInfo> sehapContextsBuff;
    static struct selabel_handle *fileContextsHandle;

private:
    int RestoreconSb(const std::string &pathname, const struct stat *sb, const std::string &apl,
                     const std::string &packageName);
    int HapContextsLookup(bool isDomain, const std::string &apl, const std::string &packageName, context_t con);
    int HapLabelLookup(const std::string &apl, const std::string &packageName, char **secontextPtr);
    int TypeSet(std::unordered_map<std::string, SehapInfo>::iterator &iter, bool isDomain, context_t con);

    static void RestoreconInit();
    static bool HapContextsLoad();
    static void HapContextsClear();
};

#endif // HAP_RESTORECON_H
