import os
import shutil

dir_path = os.path.dirname(os.path.realpath(__file__))
build_path = os.path.join(dir_path, "build")

# build and install blaze
os.system(f"cd {build_path} && meson compile && meson install")

# install into mingw64
install_path = os.path.join(dir_path, "lib")
install_lib_path = os.path.join(install_path, "lib")
install_include_path = os.path.join(install_path, "include")
install_pkgconfig_file = os.path.join(install_lib_path, "pkgconfig/blaze++.pc")

mingw64_path = "C:/msys64/mingw64"
mingw64_include_path = os.path.join(mingw64_path, "include")
mingw64_lib_path = os.path.join(mingw64_path, "lib")
mingw64_pkgconfig_path = os.path.join(mingw64_path, "lib/pkgconfig")

if os.path.exists(mingw64_path):
    # copy headers
    shutil.copytree(install_include_path, mingw64_include_path, dirs_exist_ok=True)

    # copy libs
    for file in os.listdir(install_lib_path):
        if file == "pkgconfig":
            continue


        path = os.path.join(install_lib_path, file)
        new_path = os.path.join(mingw64_lib_path, file)

        if os.path.exists(new_path):
            os.remove(new_path)

        shutil.copy(path, new_path)

    # copy pkgconfig
    shutil.copy(install_pkgconfig_file, mingw64_pkgconfig_path)