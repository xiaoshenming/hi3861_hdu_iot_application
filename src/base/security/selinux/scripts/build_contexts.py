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
import re
import shutil
from collections import defaultdict

SCRIPT_PATH = os.path.abspath(os.path.dirname(__file__))
LOCAL_PATH = os.path.abspath(os.path.join(SCRIPT_PATH, "../"))
POLICY_PATH = LOCAL_PATH + "/sepolicy"
SEHAP_CONTEXTS_PATH = LOCAL_PATH + "/sepolicy/sehap_contexts"
SERVICE_CONTEXTS_PATH = LOCAL_PATH + "/sepolicy/service_contexts"
HDF_SERVICE_CONTEXTS_PATH = LOCAL_PATH + "/sepolicy/hdf_service_contexts"
PARAMETER_CONTEXTS_PATH = LOCAL_PATH + "/sepolicy/parameter_contexts"


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--dst-file', help='the file_contexts.bin dest path', required=True)
    parser.add_argument('--tool-path',
                        help='the sefcontext_compile bin path', required=True)
    parser.add_argument('--policy-file',
                        help='the policy.31 file', required=True)
    return parser.parse_args()


def run_command(in_cmd):
    cmdstr = " ".join(in_cmd)
    rc = os.system(cmdstr)
    if rc:
        raise Exception(rc)


def traverse_folder_in_type(search_dir, file_suffix):
    """
    for special folder search_dir, find all files endwith file_suffix.
    :param search_dir: path to search
    :param file_suffix: postfix of file name
    :return: file list
    """
    policy_file_list = []
    for root, _, files in os.walk(search_dir):
        for each_file in files:
            if each_file.endswith(file_suffix):
                policy_file_list.append(os.path.join(root, each_file))
    policy_file_list.sort()
    return " ".join(str(x) for x in policy_file_list)


def combine_file_contexts(file_contexts_list, combined_file_contexts):
    cat_cmd = ["cat",
               file_contexts_list,
               ">", combined_file_contexts + "_tmp"]
    run_command(cat_cmd)

    grep_cmd = ["grep -v ^#",
                combined_file_contexts + "_tmp",
                "| grep -v ^$",
                ">", combined_file_contexts]
    run_command(grep_cmd)


def check_redefinition(contexts_file):
    type_hash = defaultdict(list)
    err = 0
    with open(contexts_file, 'r') as contexts_read:
        pattern = re.compile(r'(\S+)\s+u:object_r:\S+:s0')
        line_index = 0
        for line in contexts_read:
            line_ = line.lstrip()
            line_index += 1
            if line_.startswith('#') or line_.strip() == '':
                continue
            match = pattern.match(line_)
            if match:
                type_hash[match.group(1)].append(line_index)
            else:
                print(contexts_file + ":" +
                      str(line_index) + " format check fail")
                err = 1
        contexts_read.close()
    if err:
        print("***********************************************************")
        print("please check whether the format meets the following rules:")
        print("[required format]: * u:object_r:*:s0")
        print("***********************************************************")
        raise Exception(err)
    err = 0
    for type_key in type_hash.keys():
        if len(type_hash[type_key]) > 1:
            err = 1
            err_msg = contexts_file + ":"
            for linenum in type_hash[type_key]:
                err_msg += str(linenum) + " "
            err_msg += "'type " + str(type_key) + " is redefinition'"
            print(err_msg)
    if err:
        raise Exception(err)


def check_contexts_file(args, contexts_file):
    """
    check whether context used in contexts_file is defined in policy.31.
    :param args:
    :param contexts_file: path of contexts file
    :return:
    """
    check_redefinition(contexts_file)

    check_cmd = [args.tool_path + "/sefcontext_compile",
                 "-o", contexts_file + ".bin",
                 "-p", args.policy_file,
                 contexts_file]
    run_command(check_cmd)
    if os.path.exists(contexts_file + ".bin"):
        os.unlink(contexts_file + ".bin")
    shutil.copyfile(contexts_file, os.path.abspath(
        os.path.dirname(args.dst_file)) + "/" + os.path.basename(contexts_file))


def check_sehap_contexts(args, contexts_file, domain):
    """
    check domain or type defined in sehap_contexts.
    :param args:
    :param contexts_file: path of contexts file
    :param domain: true for domain, false for type
    :return:
    """
    shutil.copyfile(contexts_file, contexts_file + "_bk")
    err = 0
    with open(contexts_file + "_bk", 'r') as contexts_read, open(contexts_file, 'w') as contexts_write:
        pattern = re.compile(
            r'apl=(system_core|system_basic|normal)\s+(name=\S+\s+)?domain=(\S+)\s+type=(\S+)\s*\n')
        line_index = 0
        for line in contexts_read:
            line_ = line.lstrip()
            line_index += 1
            if line_.startswith('#') or line_.strip() == '':
                contexts_write.write(line)
                continue
            match = pattern.match(line_)
            if match:
                if match.group(1) == 'normal' and match.group(2) != None:
                    print(contexts_file + ":" +
                          str(line_index) + " name cannot be set while apl=normal")
                    err = 1
                if domain:
                    line = match.group(1) + " u:r:" + match.group(3) + ":s0\n"
                else:
                    line = match.group(1) + " u:object_r:" + \
                        match.group(4) + ":s0\n"
                contexts_write.write(line)
            else:
                print(contexts_file + ":" +
                      str(line_index) + " format check fail")
                err = 1
        contexts_read.close()
        contexts_write.close()
    if err:
        shutil.move(contexts_file + "_bk", contexts_file)
        print("***********************************************************")
        print("please check whether the format meets the following rules:")
        print("[required format]: apl=* name=* domain=* type=*")
        print("apl=*, apl should be one of system_core|system_basic|normal")
        print("name=*, name is 'optional'")
        print("domain=*, hapdomain selinux type")
        print("type=*, hapdatafile selinux type")
        print("***********************************************************")
        raise Exception(err)
    check_cmd = [args.tool_path + "/sefcontext_compile",
                 "-o", contexts_file + ".bin",
                 "-p", args.policy_file,
                 contexts_file]
    rc = os.system(" ".join(check_cmd))
    if rc:
        shutil.move(contexts_file + "_bk", contexts_file)
        raise Exception(rc)
    shutil.move(contexts_file + "_bk", contexts_file)
    if os.path.exists(contexts_file + ".bin"):
        os.unlink(contexts_file + ".bin")


def build_file_contetxs(args, output_path):
    file_contexts_list = traverse_folder_in_type(
        POLICY_PATH, "file_contexts")

    combined_file_contexts = output_path + "/file_contexts"
    combine_file_contexts(file_contexts_list, combined_file_contexts)

    build_tmp_cmd = ["m4",
                     "--fatal-warnings",
                     "-s", combined_file_contexts, ">", output_path + "/file_contexts.tmp"]
    run_command(build_tmp_cmd)

    build_bin_cmd = [args.tool_path + "/sefcontext_compile",
                     "-o", args.dst_file,
                     "-p", args.policy_file,
                     output_path + "/file_contexts.tmp"]
    run_command(build_bin_cmd)


def main(args):
    output_path = os.path.abspath(os.path.dirname(args.dst_file))

    build_file_contetxs(args, output_path)

    check_contexts_file(args, SERVICE_CONTEXTS_PATH)
    check_contexts_file(args, HDF_SERVICE_CONTEXTS_PATH)
    check_contexts_file(args, PARAMETER_CONTEXTS_PATH)

    check_sehap_contexts(args, SEHAP_CONTEXTS_PATH, 1)
    check_sehap_contexts(args, SEHAP_CONTEXTS_PATH, 0)
    shutil.copyfile(SEHAP_CONTEXTS_PATH, output_path + "/" +
                    os.path.basename(SEHAP_CONTEXTS_PATH))


if __name__ == "__main__":
    input_args = parse_args()
    main(input_args)
