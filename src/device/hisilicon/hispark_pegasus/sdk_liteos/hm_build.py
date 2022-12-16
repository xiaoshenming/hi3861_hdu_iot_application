import os
import sys

def check_scons():
    print("checking scons...")
    scons = os.system("scons --version")
    if scons == 0:
        print("installed scons!")
    else:
        print("scons is not installed!")
        subprocess.run(["pip", "install", "../../../../../tools/thirdparty/SCons-4.4.0-py3-none-any.whl", "--ignore-installed"])

root = sys.argv[1]

if os.path.exists("../../../../../tools"):
    os.chdir("../../../../device/hisilicon/hispark_pegasus/sdk_liteos")
    os.system(r"..\..\..\..\..\tools\thirdparty\Git\bin\sh hm_build.sh " + root + " win")
    check_scons()
else:
    os.chdir("../../../../device/hisilicon/hispark_pegasus/sdk_liteos")
    os.system(r"sh hm_build.sh " + root + " win")