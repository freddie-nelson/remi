import os
import shutil
import sys
import argparse

dir_path = os.path.dirname(os.path.realpath(__file__))
build_path = os.path.join(dir_path, "build")

clean_script = os.path.join(dir_path, "clean.py")
install_script = os.path.join(dir_path, "install.py")

targets = ["native", "wasm"]

# parse arguments
parser = argparse.ArgumentParser("setup")
parser.add_argument("--target", help="Choose the target platform to build for", choices=targets, default=targets[0])

args = parser.parse_args()

target = args.target

# remove build directory
# if os.path.exists(build_path):
#     shutil.rmtree(build_path)

print("Cleaning old setup...")
os.system(f"python {clean_script}")

# run meson setup
if target == "native":
    exit_code = os.system(f"meson setup build --wipe")
elif target == "wasm":
    cross_file = os.path.join(dir_path, f"cross/{target}.txt")
    exit_code = os.system(f"meson setup --wipe --cross-file {cross_file} build")
else:
    print("Unknown target!")
    exit_code = 1

if exit_code != 0:
    print("Failed to setup blaze!")
    exit(exit_code)

# must install blaze before setting up dev
exit_code = os.system(f"python {install_script}")
if exit_code != 0:
    print("Failed to build and install blaze!")
    print("Attempting to setup dev anyway...")

# and copy lib into dev
# dev_lib_path = os.path.join(dir_path, "dev/lib")
# if not os.path.exists(dev_lib_path):
#     os.mkdir(dev_lib_path)

# shutil.copy(os.path.join(build_path, "libblaze++.a"), os.path.join(dev_lib_path, "libblaze++.a"))

# setup dev
dev_path = os.path.join(dir_path, "dev")

if target == "native":
    exit_code = os.system(f"cd {dev_path} && meson setup build --wipe")
elif target == "wasm":
    exit_code = os.system(f"cd {dev_path} && meson setup --wipe --cross-file {cross_file} build")
else:
    print("Unknown target!")
    exit_code = 1

if exit_code != 0:
    print("Failed to setup blaze dev!")
    exit(exit_code)

