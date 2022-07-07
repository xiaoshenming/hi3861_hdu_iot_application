#!/usr/bin/env bash
#
# Copyright (c) 2021 北京万里红科技有限公司
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

set -ex

CDIR=$(dirname $(readlink -f "$0"))
RDIR=$(readlink -f "${CDIR}/../../../../")

{
  binary_dir="${RDIR}/out/ohos-arm-release/clang_x64/security/selinux/"
  sepolicy_dir=$(readlink -f "${CDIR}/../sepolicy")

  for product in 3516
  do
    command "${binary_dir}/checkpolicy" \
      "${sepolicy_dir}/sepolicy.default.${product}.conf" \
      -M -C -c 30 \
      -o "${sepolicy_dir}/sepolicy.${product}.cil"

    command "${binary_dir}/secilc" \
      "${sepolicy_dir}/sepolicy.${product}.cil" \
      -m -M true -G -c 30 -N \
      -f /dev/null \
      -o "${sepolicy_dir}/policy.${product}.31"
  done

  mv "${sepolicy_dir}/policy.${product}.31" "${sepolicy_dir}/policy.31"
}

