import os
import subprocess
import shutil

dir_path = os.path.dirname(os.path.realpath(__file__))
build_path = os.path.join(dir_path, "build")
lib_headers_path = os.path.join(dir_path, "lib/include")
lib_path = os.path.join(dir_path, "lib/lib")

dev_path = os.path.join(dir_path, "dev")
dev_build_path = os.path.join(dev_path, "build")
dev_assets_path = os.path.join(dev_path, "assets")
dev_headers_path = os.path.join(dev_path, "externals/include")

install_script = os.path.join(dir_path, "install.py")
uninstall_script = os.path.join(dir_path, "uninstall.py")

# build and install remi
print("Building and installing remi...")
exit_code = os.system(f"python {install_script}")
if exit_code != 0:
    print("Failed to build and install remi!")
    exit(exit_code)

# symlink headers into dev
# externals_headers_path = os.path.join(dir_path, "externals/include")

# for folder in [externals_headers_path, lib_headers_path]:
#     headers = os.listdir(folder)

#     for file in headers:
#         path = os.path.join(folder, file)
#         new_path = os.path.join(dev_headers_path, file)

#         print(f"Copying {path}...")

#         if os.path.exists(new_path):
#             if os.path.islink(new_path):
#                 os.unlink(new_path)
#             elif os.path.isdir(new_path):
#                 shutil.rmtree(new_path)
#             else:
#                 os.remove(new_path)
        
#         # if os.path.isdir(path):
#         #     shutil.copytree(path, new_path, dirs_exist_ok=True)
#         # else:
#         #     shutil.copy(path, new_path)

#         os.symlink(path, new_path)

# build dev
print("Building dev...")
exit_code = os.system(f"cd {dev_build_path} && meson compile")
if exit_code != 0:
    print("Failed to build dev!")
    exit(exit_code)

# copy assets
print("Copying assets...")
shutil.copytree(dev_assets_path, os.path.join(dev_build_path, "assets"), dirs_exist_ok=True)

# package assets emscripten
# if os.path.exists(os.path.join(dev_build_path, "dev.html")):
#     print("Packaging assets for emscripten...")

#     sdk = os.environ["EMSDK"]
#     file_packager = os.path.join(sdk, "upstream/emscripten/tools/file_packager.py")

#     os.chdir(dev_build_path)

#     exit_code, output = subprocess.getstatusoutput(["python", file_packager, 'assets.data', "--embed", f"{os.path.join(dev_build_path, 'assets')}@/assets"])
#     if exit_code != 0:
#         print("Failed to package assets for emscripten!")
#         exit(exit_code)
    
#     os.chdir(dir_path)
    
#     # remove first 2 lines of output as these contain a user message
#     output = "\n".join(output.split("\n")[2:])

#     file_path = os.path.join(dev_build_path, "assets.js")
#     with open(file_path, "wb") as file:
#         file.write(output.encode())
    
#     dev = os.path.join(dev_build_path, "dev.html")
#     with open(dev, "r+") as file:
#         lines = file.readlines()

#         for i in range(len(lines)):
#             if "<script" in lines[i]:
#                 lines[i] = "<script type=\"text/javascript\" src=\"assets.js\"></script>\n" + lines[i]
#                 break
        
#         file.seek(0)
#         file.write("\n".join(lines))
    
    

# copy libs
for file in os.listdir(lib_path):
    if file == "pkgconfig":
        continue

    path = os.path.join(lib_path, file)
    new_path = os.path.join(dev_build_path, file)

    if os.path.exists(new_path):
        os.remove(new_path)

    shutil.copy(path, new_path)

# copy wasm lib
# wasm_build = os.path.join(build_path, "libremi.a")
# if os.path.exists(wasm_build):
#     if not os.path.exists(os.path.join(dev_build_path, "lib")):
#         os.mkdir(os.path.join(dev_build_path, "lib"))

#     shutil.copy(wasm_build, os.path.join(dev_build_path, "lib/libremi.a"))

# uninstall remi
# print("Uninstalling remi...")
# os.system(f"python {uninstall_script}")

# run dev
print("Running dev...")

if os.path.exists(os.path.join(dev_build_path, "dev.html")):
    if os.path.exists(os.path.join(dev_build_path, "index.html")):
        os.remove(os.path.join(dev_build_path, "index.html"))
        
    os.rename(os.path.join(dev_build_path, "dev.html"), os.path.join(dev_build_path, "index.html"))

    os.system(f"start http://localhost:8000")
    os.system(f"cd {dev_build_path} && python -m http.server 8000")
elif os.path.exists(os.path.join(dev_build_path, "dev.exe")):
    os.system(f"cd {dev_build_path} && dev.exe")
else:
    os.system(f"cd {dev_build_path} && dev")


