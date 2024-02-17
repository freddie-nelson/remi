
import os
import shutil

dir_path = os.path.dirname(os.path.realpath(__file__))
build_path = os.path.join(dir_path, "build")

install_path = os.path.join(dir_path, "lib")
install_lib_path = os.path.join(install_path, "lib")
install_include_path = os.path.join(install_path, "include")
install_pkgconfig_file = os.path.join(install_lib_path, "pkgconfig/remi.pc")

mingw64_path = os.path.join("C:/", "msys64/mingw64")
mingw64_include_path = os.path.join(mingw64_path, "include")
mingw64_lib_path = os.path.join(mingw64_path, "lib")
mingw64_pkgconfig_path = os.path.join(mingw64_path, "lib/pkgconfig")

# uninstall from mingw64
if os.path.exists(mingw64_path):
    # remove headers
    for path in os.listdir(install_include_path):
        shutil.rmtree(os.path.join(mingw64_include_path, path))

    # remove libs
    for path in os.listdir(install_lib_path):
        if path == "pkgconfig":
            continue

        os.remove(os.path.join(mingw64_lib_path, path))

    # remove pkgconfig
    os.remove(os.path.join(mingw64_pkgconfig_path, "remi.pc"))

# uninstall
shutil.rmtree(install_path)