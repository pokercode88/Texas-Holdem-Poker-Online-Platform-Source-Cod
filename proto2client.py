import shutil
import os
import sys

DestDir = ""

if __name__ == "__main__":
    if len(sys.argv) > 1:
        DestDir = sys.argv[1]

    # 如果目录已经存在那么先清理       
    if os.path.isdir(DestDir):
        shutil.rmtree(DestDir)
    os.mkdir(DestDir)

    for root, dirs, files in os.walk(os.getcwd()):
        for f in files:
            # shutil.copyfile()
            if os.path.splitext(f)[1] == ".proto":
                newFileName = f + ".bytes"
                shutil.copyfile(f, os.path.join(DestDir, newFileName))

    