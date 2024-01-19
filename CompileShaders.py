# Compile all shaders in Shaders folder using glslc

import os
import shutil
import re

# Get all files in Shaders folder with extensions: .vert, .frag, .glsl
shaderFiles = []

for file in os.listdir("Shaders"):
    if file.endswith(".vert") or file.endswith(".frag") or file.endswith(".glsl"):
        shaderFiles.append(file)

# Delete all sdv files in Shaders folder before recompiling
print("Deleting old compiled shaders...", end="")

for file in os.listdir("Shaders"):
    if file.endswith(".spv"):
        os.remove("Shaders/" + file)

print("ok")

# Run glslc on all files and name output files as [filename][ShaderExtension].spv
i = 0;
totalFiles = len(shaderFiles)

for file in shaderFiles:
    i += 1
    print("Compiling " + file + " (" + str(i) + "/" + str(totalFiles) + ")...", end="")
    filename = file.split(".")[0]
    extension = file.split(".")[1]
    finalFileName = filename[0].upper() + filename[1:] + extension[0].upper() + extension[1:] + ".spv"

    if os.system("glslc Shaders/" + file + " -o Shaders/" + finalFileName):
        # Print in red color
        print("\033[91mFailed to compile " + file + "\033[0m")
        exit(1)
    else:
        print("\033[92mok\033[0m")

print("\033[92mDone!\033[0m")