# Run ldd on project executable and copy libraries to deploy folder
import os
import shutil
import re


# Find project version in CMakeLists.txt
def find_project_version():
    with open('CMakeLists.txt', 'r') as file:
        content = file.read()
        version_match = re.search(r'project\(.*VERSION\s+(\d+\.\d+\.\d+)\)', content)
        if version_match:
            return version_match.group(1)
        else:
            return None


project_version = find_project_version()

# Find project name frmo CMakeLists.txt
def find_project_name():
    with open('CMakeLists.txt', 'r') as file:
        content = file.read()
        name_match = re.search(r'project\((\w+)', content)
        if name_match:
            return name_match.group(1)
        else:
            return None


project_name = find_project_name()

# Ignore shader source files
def ignore_shader_sources(directory, filenames):
    return [name for name in filenames if name.endswith(".vert") or name.endswith(".frag") or name.endswith(".glsl")]


# Find project executables possible names (PROJECT_NAME + "Something" + "_BUILDTYPE")
def find_executables(project_name):
    executablesTypes = [""]
    executables = []
    regex = r'(?:' + project_name + r')(.*)_(?:Release|Debug|MinSizeRel).exe'

    for file in os.listdir(os.getcwd()):
        matches = re.findall(regex, file)

        if len(matches) > 0 and matches[0] not in executablesTypes:
            executablesTypes.append(matches[0])

    for type in executablesTypes:
        executable_path = ""

        for file in os.listdir(os.getcwd()):
            if file.endswith(project_name + type + "_Release.exe"):
                executable_path = os.path.join(os.getcwd(), file)
                break
            elif file.endswith(project_name + type + "_MinSizeRel.exe"):
                executable_path = os.path.join(os.getcwd(), file)
                break
            elif file.endswith(project_name + type + "_Debug.exe") and not executable_path.endswith(project_name + type + "_Release.exe") and not executable_path.endswith(project_name + type + "_MinSizeRel.exe"):
                executable_path = os.path.join(os.getcwd(), file)

        if len(executable_path) > 0:
            print("Executable found: " + executable_path)
            executables.append(executable_path)

    return executables


# Find project executable in current directory (Prefer file with "Release" postfix, otherwise "Debug")
executables = find_executables(find_project_name())

if len(executables) == 0:
    print("No executable found!")
    exit(1)

# Path to the deploy folder
deploy_folder = "./Deploy/" + project_version + "/"

# Delete folder if exists
if os.path.exists(deploy_folder):
    print("Deploy folder exists: " + deploy_folder + ", deleting...")
    shutil.rmtree(deploy_folder)

# Create deploy folder
os.makedirs(deploy_folder)

for executable_path in executables:
    # Run ldd on the executable
    ldd_output = os.popen("ldd " + executable_path).read()

    # Parse the ldd output to get the library paths
    library_paths = []
    for line in ldd_output.splitlines():
        if "=>" in line:
            # Add path to library_path if it is not equal to "not found"
            if "not found" not in line:
                matches = re.findall("^\s*([\w\.\d+-]+) => ([/\w\.\d+ -]+)(?: \(.*\))?", line)
                current_drive = os.path.splitdrive(os.getcwd())[0][0].lower()
                library_path = matches[0][1].replace("/c/", "C:/").strip().replace("/" + current_drive + "/", current_drive + ":/")

                # Append if the library not from Windows folder or related to Visual C++
                if not library_path.startswith("C:/Windows") or "msvc" in library_path or "VCRUNTIME" in library_path:
                    library_paths.append(library_path)
                    print("Library found: " + library_path)
            else:
                print("Library not found: " + line.split("=>")[0].strip())

    # Copy the libraries to the deploy folder
    for library_path in library_paths:
        print("Copying library: " + library_path + " to deploy folder: " + deploy_folder)
        shutil.copy(library_path, deploy_folder)

    # Copy the executable to the deploy folder
    shutil.copy(executable_path, deploy_folder)

# Copy assets folder if exists
if os.path.exists("./Assets/"):
    print("Copying assets folder to deploy folder: " + deploy_folder)
    shutil.copytree("./Assets/", deploy_folder + "/Assets/")

# Copy shaders binaries files if exists
if os.path.exists("./Shaders/"):
    print("Copying shaders binaries files to deploy folder: " + deploy_folder)
    shutil.copytree("./Shaders/", deploy_folder + "/Shaders/")
    # shutil.copytree("./Shaders/", deploy_folder + "/Shaders/", ignore=ignore_shader_sources)

# Archive and delete folder
shutil.make_archive("./Deploy/" + project_version, 'zip', deploy_folder)
shutil.rmtree(deploy_folder)

# Open deploy folder in explorer
os.startfile(os.path.abspath("./Deploy"))
