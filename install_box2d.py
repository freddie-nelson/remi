import os
import urllib.request
import zipfile
import tarfile

def download_and_extract(url, filename, output_dir, delete_after_extract=True):
    if not os.path.exists(filename):
        print(f"Downloading {filename}...")
        urllib.request.urlretrieve(url, filename)

        print(f"Extracting {filename}...")

        if filename.endswith(".tar.gz"):
            with tarfile.open(filename, "r:gz") as tar_ref:
                tar_ref.extractall(output_dir)    
        elif filename.endswith(".zip"):
            with zipfile.ZipFile(filename, "r") as zip_ref:
                zip_ref.extractall(output_dir)
        else: 
            raise ValueError(f"Unknown file type: {filename}")
        
        if delete_after_extract:
            os.remove(filename)
    else:
        print(f"{filename} already installed!")

dir_path = os.path.dirname(os.path.realpath(__file__))
output_dir = os.path.join(dir_path, "externals/libs")
output = os.path.join(output_dir, "box2d")

url = "https://github.com/erincatto/box2d/archive/v2.4.1/box2d-2.4.1.tar.gz"
meson_patch = "https://wrapdb.mesonbuild.com/v2/box2d_2.4.1-3/get_patch"

download_and_extract(url, os.path.join(output_dir, "out.tar.gz"), output_dir)
# download_and_extract(meson_patch, os.path.join(output_dir, "patch.zip"), output_dir)


