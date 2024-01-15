import os
import urllib.request
import zipfile

dir_path = os.path.dirname(os.path.realpath(__file__))
externals_path = os.path.join(dir_path, "externals")
include_path = os.path.join(externals_path, "include")
boost_path = os.path.join(include_path, "boost")    

boost_url = "https://github.com/emscripten-ports/boost/releases/download/boost-1.83.0/boost-headers-1.83.0.zip"

if not os.path.exists(boost_path):
    print("Downloading boost headers...")
    urllib.request.urlretrieve(boost_url, "boost.zip")

    print("Extracting boost headers...")
    with zipfile.ZipFile("boost.zip", "r") as zip_ref:
        zip_ref.extractall(include_path)
else:
    print("Boost already installed!")



