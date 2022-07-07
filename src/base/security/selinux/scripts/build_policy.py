#!/usr/bin/env python
# coding: utf-8

"""
Copyright (c) 2021-2022 Huawei Device Co., Ltd.
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

"""

import os
import argparse

SCRIPT_PATH = os.path.abspath(os.path.dirname(__file__))
LOCAL_PATH = os.path.abspath(os.path.join(SCRIPT_PATH, "../"))
POLICY_PATH = LOCAL_PATH + "/sepolicy/ohos_policy"

# list of all macros and te for sepolicy build
SEPOLICY_TYPE_LIST = ["security_classes",
                      "initial_sids",
                      "access_vectors",
                      "glb_perm_def.spt",
                      "glb_never_def.spt",
                      "mls",
                      "policy_cap",
                      "glb_te_def.spt",
                      "attributes",
                      ".te",
                      "glb_roles.spt",
                      "users",
                      "initial_sid_contexts",
                      "fs_use",
                      "virtfs_contexts",
                      ]


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--dst-file', help='the policy dest path', required=True)
    parser.add_argument('--tool-path',
                        help='the policy tool bin path', required=True)
    return parser.parse_args()


def traverse_folder_in_dir_name(search_dir, folder_suffix):
    folder_list = []
    for root, dirs, _ in os.walk(search_dir):
        for dir_i in dirs:
            if dir_i == folder_suffix:
                folder_list.append(os.path.join(root, dir_i))
    return folder_list


def traverse_folder_in_type(search_dir, file_suffix):
    policy_file_list = []
    for root, _, files in os.walk(search_dir):
        for each_file in files:
            if each_file.endswith(file_suffix):
                policy_file_list.append(os.path.join(root, each_file))
    policy_file_list.sort()
    return " ".join(str(x) for x in policy_file_list)


def traverse_file_in_each_type(folder_list, sepolicy_type_list):
    policy_files = ""
    for policy_type in sepolicy_type_list:
        for folder in folder_list:
            policy_files += traverse_folder_in_type(folder, policy_type) + " "
    return policy_files


def run_command(in_cmd):
    cmdstr = " ".join(in_cmd)
    rc = os.system(cmdstr)
    if rc:
        raise Exception(rc)


def build_conf(output_conf, input_policy_file_list):
    build_conf_cmd = ["m4",
                      "--fatal-warnings",
                      "-s", input_policy_file_list, ">", output_conf]
    run_command(build_conf_cmd)


def build_cil(args, output_cil, input_conf):
    check_policy_cmd = [args.tool_path + "/checkpolicy",
                        input_conf,
                        "-M -C -c 31",
                        "-o " + output_cil]
    run_command(check_policy_cmd)


def build_policy(args, output_policy, input_cil):
    build_policy_cmd = [args.tool_path + "/secilc",
                        input_cil,
                        "-m -M true -G -c 31 -N",
                        "-f /dev/null",
                        "-o " + output_policy]
    run_command(build_policy_cmd)


def main(args):
    output_path = os.path.abspath(os.path.dirname(args.dst_file))

    base_policy = [LOCAL_PATH + "/sepolicy/base"]
    public_policy = traverse_folder_in_dir_name(POLICY_PATH, "public")
    system_policy = traverse_folder_in_dir_name(POLICY_PATH, "system")
    vendor_policy = traverse_folder_in_dir_name(POLICY_PATH, "vendor")

    # list of all policy folders
    folder_list = base_policy + public_policy + system_policy + vendor_policy

    # list of all policy files
    policy_file_list = traverse_file_in_each_type(
        folder_list, SEPOLICY_TYPE_LIST)

    # build ohos.conf
    output_ohos_conf = output_path + "/ohos.conf"
    build_conf(output_ohos_conf, policy_file_list)

    # build ohos.cil
    ohos_cil_path = output_path + "/ohos.cil"
    build_cil(args, ohos_cil_path, output_ohos_conf)

    build_policy(args, args.dst_file, ohos_cil_path)


if __name__ == "__main__":
    input_args = parse_args()
    main(input_args)
