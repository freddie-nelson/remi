import os
import shutil

dir_path = os.path.dirname(os.path.realpath(__file__))
lib_path = os.path.join(dir_path, "lib")
build_path = os.path.join(dir_path, "build")
dev_build_path = os.path.join(dir_path, "dev/build")

print("Cleaning build...")
if os.path.exists(build_path):
    shutil.rmtree(build_path)

print("Cleaning dev build...")
if os.path.exists(dev_build_path):
    shutil.rmtree(dev_build_path)

print("Cleaning lib...")
if os.path.exists(lib_path):
    shutil.rmtree(lib_path)