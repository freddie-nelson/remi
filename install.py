import os
import shutil

dir_path = os.path.dirname(os.path.realpath(__file__))
build_path = os.path.join(dir_path, "build")

install_path = os.path.join(dir_path, "lib")
install_lib_path = os.path.join(install_path, "lib")
install_include_path = os.path.join(install_path, "include")
install_pkgconfig_file = os.path.join(install_lib_path, "pkgconfig/remi.pc")

# remove old install
shutil.rmtree(install_path, ignore_errors=True)

# build and install remi
exit_code = os.system(f"cd {build_path} && meson compile && meson install")
if exit_code != 0:
    print("Failed to build and install remi!")
    exit(exit_code)


# # install into mingw64
# if not os.path.exists(install_lib_path):
#     print("No install output found!")
#     print("May be non issue depending on target platform.")
#     print("Exiting install script...")
#     exit(0)

# # msys64_envs = ["mingw64", "ucrt64"]
# msys64_envs = ["mingw64"]

# for env in msys64_envs:
#     path = f"C:/msys64/{env}"
#     include_path = os.path.join(path, "include")
#     lib_path = os.path.join(path, "lib")
#     pkgconfig_path = os.path.join(path, "lib/pkgconfig")

#     if os.path.exists(path):
#         # copy headers
#         shutil.copytree(install_include_path, include_path, dirs_exist_ok=True)

#         # copy libs
#         for file in os.listdir(install_lib_path):
#             if file == "pkgconfig":
#                 continue


#             path = os.path.join(install_lib_path, file)
#             new_path = os.path.join(lib_path, file)

#             if os.path.exists(new_path):
#                 os.remove(new_path)

#             shutil.copy(path, new_path)

#         # copy pkgconfig
#         shutil.copy(install_pkgconfig_file, pkgconfig_path)