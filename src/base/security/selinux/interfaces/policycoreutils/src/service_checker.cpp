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

#include "service_checker.h"
#include <selinux/label.h>
#include <selinux_internal.h>
#include <sstream>
#include <fstream>
#include "selinux_klog.h"
#include "selinux_error.h"
#include "callbacks.h"
#include "securec.h"

using namespace Selinux;

namespace {
static const std::string SERVICE_CONTEXTS_FILE = "/system/etc/selinux/targeted/contexts/service_contexts";
static const std::string HDF_SERVICE_CONTEXTS_FILE = "/system/etc/selinux/targeted/contexts/hdf_service_contexts";
static const std::string OBJECT_PREFIX = "u:object_r:";
static const std::string DEFAULT_CONTEXT = "u:object_r:default_service:s0";
static const std::string DEFAULT_HDF_CONTEXT = "u:object_r:default_hdf_service:s0";
static const int CONTEXTS_LENGTH_MIN = 16; // sizeof("x u:object_r:x:s0")
static const int CONTEXTS_LENGTH_MAX = 1024;
static pthread_once_t FC_ONCE = PTHREAD_ONCE_INIT;
} // namespace

extern "C" int HdfListServiceCheck(pid_t callingPid)
{
    return ServiceChecker::GetInstance().ListServiceCheck(callingPid);
}

extern "C" int HdfGetServiceCheck(pid_t callingPid, const char *serviceName)
{
    if (serviceName == nullptr) {
        return -SELINUX_PTR_NULL;
    }
    return ServiceChecker::GetInstance().GetServiceCheck(callingPid, serviceName);
}

extern "C" int HdfAddServiceCheck(pid_t callingPid, const char *serviceName)
{
    if (serviceName == nullptr) {
        return -SELINUX_PTR_NULL;
    }
    return ServiceChecker::GetInstance().AddServiceCheck(callingPid, serviceName);
}

struct AuditMsg {
    pid_t pid;
    const char *name;
};

static int SelinuxAuditCallback(void *data, security_class_t cls, char *buf, size_t len)
{
    if (data == nullptr || buf == nullptr) {
        return -1;
    }
    auto *msg = reinterpret_cast<AuditMsg *>(data);
    if (!msg->name) {
        selinux_log(SELINUX_ERROR, "audit msg invalid argument\n");
        return -1;
    }
    if (snprintf_s(buf, len, len - 1, "service=%s pid=%d", msg->name, msg->pid) <= 0) {
        return -1;
    }
    return 0;
}

static void SelinuxSetCallback()
{
    union selinux_callback cb;
    cb.func_log = SelinuKLog;
    selinux_set_callback(SELINUX_CB_LOG, cb);
    cb.func_audit = SelinuxAuditCallback;
    selinux_set_callback(SELINUX_CB_AUDIT, cb);
}

static bool CouldSkip(const std::string &line)
{
    if (line.size() < CONTEXTS_LENGTH_MIN || line.size() > CONTEXTS_LENGTH_MAX) {
        return true;
    }
    int i = 0;
    while (isspace(line[i])) {
        i++;
    }
    if (line[i] == '#') {
        return true;
    }
    if (line.find(OBJECT_PREFIX) == line.npos) {
        return true;
    }
    return false;
}

static bool StartWith(const std::string &dst, const std::string &prefix)
{
    return dst.compare(0, prefix.size(), prefix) == 0;
}

static struct ServiceInfo DecodeString(const std::string &line)
{
    std::stringstream input(line);
    struct ServiceInfo contextBuff = {"", ""};
    std::string name;
    if (input >> name) {
        contextBuff.serviceName = name;
    }
    std::string context;
    if (input >> context) {
        if (StartWith(context, OBJECT_PREFIX)) {
            contextBuff.serviceContext = context;
        }
    }
    return contextBuff;
}

static int CheckServiceNameValid(const std::string &serviceName)
{
    if (serviceName.empty() || serviceName[0] == '.') {
        return -SELINUX_ARG_INVALID;
    }
    return SELINUX_SUCC;
}

void ServiceChecker::SetSelinuxLogCallback()
{
    SetSelinuKLogLevel(SELINUX_KINFO);
    __selinux_once(FC_ONCE, SelinuxSetCallback);
    return;
}

bool ServiceChecker::ServiceContextsLoad()
{
    // load service_contexts file
    std::string name;
    if (isHdf_) {
        name = HDF_SERVICE_CONTEXTS_FILE;
    } else {
        name = SERVICE_CONTEXTS_FILE;
    }
    std::ifstream contextsFile(name);
    if (contextsFile) {
        int lineNum = 0;
        std::string line;
        while (getline(contextsFile, line)) {
            lineNum++;
            if (CouldSkip(line))
                continue;
            struct ServiceInfo tmpInfo = DecodeString(line);
            if (!tmpInfo.serviceContext.empty() && !tmpInfo.serviceName.empty()) {
                serviceMap.emplace(tmpInfo.serviceName, tmpInfo);
            } else {
                selinux_log(SELINUX_ERROR, "service_contexts read fail in line %d\n", lineNum);
            }
        }
    } else {
        selinux_log(SELINUX_ERROR, "Load service_contexts fail, no such file: %s\n", name.c_str());
        return false;
    }
    selinux_log(SELINUX_INFO, "Load service_contexts succes: %s\n", name.c_str());
    contextsFile.close();
    return true;
}

int ServiceChecker::GetServiceContext(const std::string &serviceName, std::string &context)
{
    if (CheckServiceNameValid(serviceName) != 0) {
        selinux_log(SELINUX_ERROR, "serviceName invalid!\n");
        return -SELINUX_ARG_INVALID;
    }

    if (serviceMap.empty()) {
        if (!ServiceContextsLoad()) {
            return -SELINUX_CONTEXTS_FILE_LOAD_ERROR;
        }
    }

    auto iter = serviceMap.find(serviceName);
    if (iter != serviceMap.end()) {
        context = iter->second.serviceContext;
    } else {
        context = isHdf_ ? DEFAULT_HDF_CONTEXT : DEFAULT_CONTEXT;
    }
    selinux_log(SELINUX_INFO, "find context: %s\n", context.c_str());
    return SELINUX_SUCC;
}

static int GetCallingContext(const pid_t &pid, std::string &context)
{
    char *srcContext = nullptr;
    int rc = getpidcon(pid, &srcContext);
    if (rc < 0) {
        selinux_log(SELINUX_ERROR, "getpidcon failed!\n");
        return -SELINUX_GET_CONTEXT_ERROR;
    }
    context = std::string(srcContext);
    freecon(srcContext);
    return SELINUX_SUCC;
}

static int GetThisContext(std::string &context)
{
    char *con = nullptr;
    int rc = getcon(&con);
    if (rc < 0) {
        selinux_log(SELINUX_ERROR, "getcon failed!\n");
        return -SELINUX_GET_CONTEXT_ERROR;
    }
    context = std::string(con);
    freecon(con);
    return SELINUX_SUCC;
}

int ServiceChecker::CheckPerm(const pid_t &callingPid, const std::string &serviceName, std::string action)
{
    std::string srcContext = "";
    int ret = GetCallingContext(callingPid, srcContext);
    if (ret != SELINUX_SUCC) {
        return ret;
    }
    if (security_check_context(srcContext.c_str()) < 0) {
        selinux_log(SELINUX_ERROR, "context: %s, %s\n", srcContext.c_str(), GetErrStr(SELINUX_CHECK_CONTEXT_ERROR));
        return -SELINUX_CHECK_CONTEXT_ERROR;
    }
    std::string destContext = "";
    if (action == "list") {
        ret = GetThisContext(destContext);
    } else {
        ret = GetServiceContext(serviceName, destContext);
    }
    if (ret != SELINUX_SUCC) {
        return ret;
    }
    if (security_check_context(destContext.c_str()) < 0) {
        selinux_log(SELINUX_ERROR, "context: %s, %s\n", destContext.c_str(), GetErrStr(SELINUX_CHECK_CONTEXT_ERROR));
        return -SELINUX_CHECK_CONTEXT_ERROR;
    }

    AuditMsg msg;
    msg.name = serviceName.c_str();
    msg.pid = callingPid;
    selinux_log(SELINUX_INFO, "srcContext[%s] %s service[%s] destContext[%s]\n", srcContext.c_str(), action.c_str(),
                msg.name, destContext.c_str());
    int res =
        selinux_check_access(srcContext.c_str(), destContext.c_str(), serviceClass_.c_str(), action.c_str(), &msg);
    return res == 0 ? SELINUX_SUCC : -SELINUX_PERMISSION_DENY;
}

int ServiceChecker::ListServiceCheck(const pid_t &callingPid)
{
    return CheckPerm(callingPid, serviceClass_, "list");
}

int ServiceChecker::GetServiceCheck(const pid_t &callingPid, const std::string &serviceName)
{
    return CheckPerm(callingPid, serviceName, "get");
}

int ServiceChecker::GetRemoteServiceCheck(const pid_t &callingPid, const std::string &remoteServiceName)
{
    if (isHdf_) {
        selinux_log(SELINUX_ERROR, "hdf service has no permission to get remote!\n");
        return -SELINUX_PERMISSION_DENY;
    }
    return CheckPerm(callingPid, remoteServiceName, "get_remote");
}

int ServiceChecker::AddServiceCheck(const pid_t &callingPid, const std::string &serviceName)
{
    return CheckPerm(callingPid, serviceName, "add");
}

ServiceChecker& ServiceChecker::GetInstance()
{
    static ServiceChecker instance(true);
    return instance;
}
