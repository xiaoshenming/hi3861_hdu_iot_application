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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_BUNDLEMGR_BUNDLE_MGR_CLIENT_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_BUNDLEMGR_BUNDLE_MGR_CLIENT_H

#include "bundle_constants.h"
#include "bundle_info.h"
#include "extension_ability_info.h"
#include "hap_module_info.h"

namespace OHOS {
namespace AppExecFwk {
class BundleMgrClientImpl;

class BundleMgrClient {
public:
    BundleMgrClient();
    virtual ~BundleMgrClient();

    bool GetBundleNameForUid(const int uid, std::string &bundleName);
    bool GetBundleInfo(const std::string &bundleName, const BundleFlag flag, BundleInfo &bundleInfo,
        int32_t userId = Constants::UNSPECIFIED_USERID);

    /**
     * @brief Obtain the profile which are deploied in the Metadata in the bundle.
     * @param bundleName Indicates the bundle name of the bundle.
     * @param hapName Indicates the hap name of the bundle.
     * @param hapModuleInfo Indicates the information of the hap.
     * @return Returns true if this function is successfully called; returns false otherwise.
     */
    bool GetHapModuleInfo(const std::string &bundleName, const std::string &hapName, HapModuleInfo &hapModuleInfo);
    /**
     * @brief Obtain the profile which are deploied in the Metadata in the bundle.
     * @param hapModuleInfo Indicates the information of a hap of this bundle.
     * @param MetadataName Indicates the name of the Metadata.
     * @param profileInfos Indicates the obtained profiles in json string.
     * @return Returns true if this function is successfully called; returns false otherwise.
     */
    bool GetResConfigFile(const HapModuleInfo &hapModuleInfo, const std::string &metadataName,
    std::vector<std::string> &profileInfos) const;
    /**
     * @brief Obtain the profile which are deploied in the Metadata in the bundle.
     * @param extensionInfo Indicates the information of the extension info of the bundle.
     * @param MetadataName Indicates the name of the Metadata.
     * @param profileInfos Indicates the obtained profiles in json string.
     * @return Returns true if this function is successfully called; returns false otherwise.
     */
    bool GetResConfigFile(const ExtensionAbilityInfo &extensionInfo, const std::string &metadataName,
        std::vector<std::string> &profileInfos) const;
    /**
     * @brief Obtain the profile which are deploied in the Metadata in the bundle.
     * @param abilityInfo Indicates the information of the ability info of the bundle.
     * @param MetadataName Indicates the name of the Metadata.
     * @param profileInfos Indicates the obtained profiles in json string.
     * @return Returns true if this function is successfully called; returns false otherwise.
     */
    bool GetResConfigFile(const AbilityInfo &abilityInfo, const std::string &metadataName,
        std::vector<std::string> &profileInfos) const;

    std::vector<std::string> GetAccessibleAppCodePaths(int32_t userId);

    /**
     * @brief Install sandbox application.
     * @param bundleName Indicates the bundle name of the sandbox application to be install.
     * @param userId Indicates the sandbox application will be installed under which user id.
     * @return Returns true if the sandbox application is installed successfully; returns false otherwise.
     */
    bool InstallSandboxApp(const std::string &bundleName, int32_t userId);

    /**
     * @brief Uninstall sandbox application.
     * @param bundleName Indicates the bundle name of the sandbox application to be install.
     * @param appIndex Indicates application index of the sandbox application.
     * @param userId Indicates the sandbox application will be uninstall under which user id.
     * @return Returns true if the sandbox application is installed successfully; returns false otherwise.
     */
    bool UninstallSandboxApp(const std::string &bundleName, int32_t appIndex, int32_t userId);

    bool GetSandboxAppBundleInfo(const std::string &bundleName, int32_t appIndex, int32_t userId, BundleInfo &info);
private:
    std::shared_ptr<BundleMgrClientImpl> impl_;
};
}  // namespace AppExecFwk
}  // namespace OHOS

#endif  // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_BUNDLEMGR_BUNDLE_MGR_CLIENT_H
