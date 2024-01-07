import os
import shutil
import sys
import argparse

dir_path = os.path.dirname(os.path.realpath(__file__))
build_path = os.path.join(dir_path, "build")

targets = ["native", "wasm"]

# parse arguments
parser = argparse.ArgumentParser("setup")
parser.add_argument("--target", help="Choose the target platform to build for", choices=targets, default=targets[0])

args = parser.parse_args()

target = args.target

# remove build directory
if os.path.exists(build_path):
    shutil.rmtree(build_path)

# run meson setup
if target == "native":
    exit_code = os.system(f"meson setup build")
else:
    cross_file = os.path.join(dir_path, f"cross-{target}.txt")
    exit_code = os.system(f"meson setup --cross-file {cross_file} build")

if exit_code != 0:
    print("Failed to setup blaze!")
    exit(exit_code)

