#!/usr/bin/python3

# Function: extract hi3861 related code from OpenHarmony full-code repository LTS v3.0.5

import sys, os, subprocess

dir_from = "/mnt/local/ohos_3861/project_iot_3861/code_from/"
location_openharmony_full = os.path.join(dir_from, "code-v3.0.5-LTS/OpenHarmony")
location_demo = os.path.join(dir_from, "demo")
location_notice = os.path.join(dir_from, "NOTICE")
location_tar = "/mnt/local/ohos_3861/project_iot_3861/code_tar"

dirs_to_del_recur = (".git", ".gitee", ".gitattributes")
dirs_to_del_sep = ("device/hisilicon/hispark_pegasus/sdk_liteos/third_party/u-boot-v2019.07/u-boot-v2019.07",
            "device/hisilicon/hispark_pegasus/sdk_liteos/third_party/u-boot-v2019.07/u-boot-v2019.07/u-boot-v2019.07.tar.gz",
            "device/hisilicon/hispark_pegasus/sdk_liteos/output",
        )
nessesary_branches = (
        "base/hiviewdfx/",
        "base/security/",
        "base/startup/",
        "base/update/ota_lite/",
        "base/global/i18n_standard/",
        "base/global/i18n_lite/",
        "base/global/resmgr_lite/",
        "base/iot_hardware/",
        "base/powermgr/battery_lite/",
        "base/powermgr/powermgr_lite/",
        "base/sensors/",
        "device/hisilicon/hispark_pegasus/",
        "foundation/appexecfwk/appexecfwk_lite",
        "foundation/communication/softbus_lite",
        "foundation/communication/wifi",
        "foundation/communication/wifi_aware",
        "foundation/communication/wifi_lite",
        "foundation/distributeddatamgr/appdatamgr",
        "foundation/distributedschedule/samgr_lite",
        "test/xts/acts/build_lite",
        "test/xts/acts/communication_lite",
        "test/xts/acts/utils_lite",
        "test/xts/acts/startup_lite",
        "test/xts/acts/iot_hardware_lite",
        "test/xts/acts/security_lite",
        "test/xts/acts/hiviewdfx_lite",
        "test/xts/acts/distributed_schedule_lite",
        "test/xts/acts/update_lite",
        "test/xts/tools/",
        "test/xdevice/",
        "third_party/cJSON/",
        "third_party/jinja2/",
        "third_party/mbedtls/",
        "third_party/unity/",
        "utils/native/lite/",
        "vendor/hisilicon/hispark_pegasus/",
        "build/",
        "applications/sample/wifi-iot/",
        "domains/iot/",
        "prebuilts/build-tools/linux-x86/",
        "prebuilts/lite/sysroot/",
        "productdefine/common/",
        )

def main():
    def exec_cmd(cmd):
        print(cmd)
        try:
            p = subprocess.run(cmd, shell=True)
        except Exception as e:
            print(e)
            sys.exit(p.returncode)

    # pick hi3861 necessary branches to target
    print("--- 1. pick necessary branches from OpenHarmony full code repository to target folder:")
    cmd_rsync = ['rsync -aR {:s} {:s}'.format(branch, location_tar) \
                    for branch in nessesary_branches]
    cmd = 'cd {:s} && '.format(location_openharmony_full) + ' && '.join(cmd_rsync)
    exec_cmd(cmd)

    # merge demo to target
    print("--- 2. copy demo to target folder:")
    cmd = "rsync -a {:s} {:s}".format(location_demo, os.path.join(location_tar, 'vendor/hisilicon/hispark_pegasus'))
    exec_cmd(cmd)

    # copy Notice and License File to target
    print("--- 3. copy notice to target folder:")
    cmd = "rsync -a {:s} {:s}".format(location_notice, location_tar)
    exec_cmd(cmd)
    
    # rm .git .gitee files from target folder
    print("--- 4. delete redundant files")

    cmd_del = ['find -name {} | xargs rm -rf'.format(dd) for dd in dirs_to_del_recur]
    cmd = "cd {} && ".format(location_tar) + " && ".join(cmd_del)
    exec_cmd(cmd)

    cmd_del = ['rm -rf {}'.format(dd) for dd in dirs_to_del_sep]
    cmd = "cd {} && ".format(location_tar) + " && ".join(cmd_del)
    exec_cmd(cmd)
    
    return 0
    


if "__main__" == __name__:
    sys.exit(main())
