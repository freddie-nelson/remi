import os
import shutil

dir_path = os.path.dirname(os.path.realpath(__file__))
build_path = os.path.join(dir_path, "build")

exit_code = os.system(f"cd {build_path} && meson compile")
if exit_code != 0:
    print("Failed to build blaze!")
    exit(exit_code)

