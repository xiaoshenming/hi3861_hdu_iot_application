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

#include "bundle_installer_proxy.h"

#include "bytrace.h"
#include "ipc_types.h"
#include "parcel.h"
#include "string_ex.h"

#include "app_log_wrapper.h"

namespace OHOS {
namespace AppExecFwk {

BundleInstallerProxy::BundleInstallerProxy(const sptr<IRemoteObject> &object) : IRemoteProxy<IBundleInstaller>(object)
{
    APP_LOGI("create bundle installer proxy instance");
}

BundleInstallerProxy::~BundleInstallerProxy()
{
    APP_LOGI("destroy bundle installer proxy instance");
}

bool BundleInstallerProxy::Install(
    const std::string &bundlePath, const InstallParam &installParam, const sptr<IStatusReceiver> &statusReceiver)
{
    BYTRACE_NAME(BYTRACE_TAG_APP, __PRETTY_FUNCTION__);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    PARCEL_WRITE_INTERFACE_TOKEN(data, GetDescriptor());
    PARCEL_WRITE(data, String16, Str8ToStr16(bundlePath));
    PARCEL_WRITE(data, Parcelable, &installParam);

    if (!statusReceiver) {
        APP_LOGE("fail to install, for statusReceiver is nullptr");
        return false;
    }
    if (!data.WriteObject<IRemoteObject>(statusReceiver->AsObject())) {
        APP_LOGE("write parcel failed");
        return false;
    }

    return SendInstallRequest(IBundleInstaller::Message::INSTALL, data, reply, option);
}

bool BundleInstallerProxy::Install(const std::vector<std::string> &bundleFilePaths, const InstallParam &installParam,
    const sptr<IStatusReceiver> &statusReceiver)
{
    BYTRACE_NAME(BYTRACE_TAG_APP, __PRETTY_FUNCTION__);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    PARCEL_WRITE_INTERFACE_TOKEN(data, GetDescriptor());
    auto size = bundleFilePaths.size();
    PARCEL_WRITE(data, Int32, size);
    for (uint32_t i = 0; i < size; ++i) {
        PARCEL_WRITE(data, String16, Str8ToStr16(bundleFilePaths[i]));
    }
    PARCEL_WRITE(data, Parcelable, &installParam);

    if (!statusReceiver) {
        APP_LOGE("fail to install, for statusReceiver is nullptr");
        return false;
    }
    if (!data.WriteObject<IRemoteObject>(statusReceiver->AsObject())) {
        APP_LOGE("write parcel failed");
        return false;
    }

    return SendInstallRequest(IBundleInstaller::Message::INSTALL_MULTIPLE_HAPS, data, reply,
        option);
}

bool BundleInstallerProxy::Recover(const std::string &bundleName,
    const InstallParam &installParam, const sptr<IStatusReceiver> &statusReceiver)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    PARCEL_WRITE_INTERFACE_TOKEN(data, GetDescriptor());
    PARCEL_WRITE(data, String16, Str8ToStr16(bundleName));
    PARCEL_WRITE(data, Parcelable, &installParam);

    if (!statusReceiver) {
        APP_LOGE("fail to install, for statusReceiver is nullptr");
        return false;
    }
    if (!data.WriteObject<IRemoteObject>(statusReceiver->AsObject())) {
        APP_LOGE("write parcel failed");
        return false;
    }

    return SendInstallRequest(IBundleInstaller::Message::RECOVER, data, reply,
        option);
}

bool BundleInstallerProxy::Uninstall(
    const std::string &bundleName, const InstallParam &installParam, const sptr<IStatusReceiver> &statusReceiver)
{
    BYTRACE_NAME(BYTRACE_TAG_APP, __PRETTY_FUNCTION__);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    PARCEL_WRITE_INTERFACE_TOKEN(data, GetDescriptor());
    PARCEL_WRITE(data, String16, Str8ToStr16(bundleName));
    PARCEL_WRITE(data, Parcelable, &installParam);
    if (!statusReceiver) {
        APP_LOGE("fail to uninstall, for statusReceiver is nullptr");
        return false;
    }
    if (!data.WriteObject<IRemoteObject>(statusReceiver->AsObject())) {
        APP_LOGE("write parcel failed");
        return false;
    }

    return SendInstallRequest(IBundleInstaller::Message::UNINSTALL, data, reply, option);
}

bool BundleInstallerProxy::Uninstall(const std::string &bundleName, const std::string &modulePackage,
    const InstallParam &installParam, const sptr<IStatusReceiver> &statusReceiver)
{
    BYTRACE_NAME(BYTRACE_TAG_APP, __PRETTY_FUNCTION__);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    PARCEL_WRITE_INTERFACE_TOKEN(data, GetDescriptor());
    PARCEL_WRITE(data, String16, Str8ToStr16(bundleName));
    PARCEL_WRITE(data, String16, Str8ToStr16(modulePackage));
    PARCEL_WRITE(data, Parcelable, &installParam);
    if (!statusReceiver) {
        APP_LOGE("fail to uninstall, for statusReceiver is nullptr");
        return false;
    }
    if (!data.WriteObject<IRemoteObject>(statusReceiver->AsObject())) {
        APP_LOGE("write parcel failed");
        return false;
    }

    return SendInstallRequest(IBundleInstaller::Message::UNINSTALL_MODULE, data, reply, option);
}

bool BundleInstallerProxy::InstallSandboxApp(const std::string &bundleName, int32_t userId)
{
    BYTRACE_NAME(BYTRACE_TAG_APP, __PRETTY_FUNCTION__);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    PARCEL_WRITE_INTERFACE_TOKEN(data, GetDescriptor());
    PARCEL_WRITE(data, String16, Str8ToStr16(bundleName));
    PARCEL_WRITE(data, Int32, userId);

    auto ret =
        SendInstallRequest(IBundleInstaller::Message::INSTALL_SANDBOX_APP, data, reply, option);
    if (!ret) {
        APP_LOGE("install sandbox app failed due to send request fail");
        return false;
    }
    return reply.ReadBool();
}

bool BundleInstallerProxy::UninstallSandboxApp(const std::string &bundleName, int32_t appIndex, int32_t userId)
{
    BYTRACE_NAME(BYTRACE_TAG_APP, __PRETTY_FUNCTION__);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    PARCEL_WRITE_INTERFACE_TOKEN(data, GetDescriptor());
    PARCEL_WRITE(data, String16, Str8ToStr16(bundleName));
    PARCEL_WRITE(data, Int32, appIndex);
    PARCEL_WRITE(data, Int32, userId);

    auto ret =
        SendInstallRequest(IBundleInstaller::Message::UNINSTALL_SANDBOX_APP, data, reply, option);
    if (!ret) {
        APP_LOGE("uninstall sandbox app failed due to send request fail");
        return false;
    }
    return reply.ReadBool();
}

bool BundleInstallerProxy::SendInstallRequest(const uint32_t& code, MessageParcel& data, MessageParcel& reply,
    MessageOption& option)
{
    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        APP_LOGE("fail to uninstall, for Remote() is nullptr");
        return false;
    }

    int32_t ret = remote->SendRequest(code, data, reply, option);
    if (ret != NO_ERROR) {
        APP_LOGE("fail to sendRequest, for transact is failed and error code is: %{public}d", ret);
        return false;
    }
    return true;
}
}  // namespace AppExecFwk
}  // namespace OHOS