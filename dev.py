import os
import shutil

dir_path = os.path.dirname(os.path.realpath(__file__))
build_path = os.path.join(dir_path, "build")
lib_path = os.path.join(dir_path, "lib/lib")

dev_path = os.path.join(dir_path, "dev")
dev_build_path = os.path.join(dev_path, "build")
dev_assets_path = os.path.join(dev_path, "assets")

install_script = os.path.join(dir_path, "install.py")
uninstall_script = os.path.join(dir_path, "uninstall.py")

# build and install blaze
print("Building and installing blaze...")
exit_code = os.system(f"python {install_script}")
if exit_code != 0:
    print("Failed to build and install blaze!")
    exit(exit_code)

# build dev
print("Building dev...")
exit_code = os.system(f"cd {dev_build_path} && meson compile")
if exit_code != 0:
    print("Failed to build dev!")
    exit(exit_code)

# copy assets
print("Copying assets...")
shutil.copytree(dev_assets_path, os.path.join(dev_build_path, "assets"), dirs_exist_ok=True)

# copy libs
for file in os.listdir(lib_path):
    if file == "pkgconfig":
        continue

    path = os.path.join(lib_path, file)
    new_path = os.path.join(dev_build_path, file)

    if os.path.exists(new_path):
        os.remove(new_path)

    shutil.copy(path, new_path)

# uninstall blaze
# print("Uninstalling blaze...")
# os.system(f"python {uninstall_script}")

# run dev
print("Running dev...")

if os.path.exists(os.path.join(dev_build_path, "dev.exe")):
    os.system(f"cd {dev_build_path} && dev.exe")
else:
    os.system(f"cd {dev_build_path} && dev")


