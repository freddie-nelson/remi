import os
import shutil
import json
import argparse
import subprocess

base_dir = os.path.dirname(os.path.abspath(__file__))
wasm_cross_file = os.path.join(base_dir, 'cross/wasm.txt')

examples_dir = os.path.join(base_dir, 'examples')

examples_build_dir_name = '__build'
examples_build_dir = os.path.join(examples_dir, examples_build_dir_name)
examples_helpers_dir = os.path.join(examples_dir, '__helpers')
examples_helpers_assets_dir = os.path.join(examples_helpers_dir, 'assets')

def get_examples_names():
    return [d for d in os.listdir(examples_dir) if not d.startswith("__") and os.path.isdir(os.path.join(examples_dir, d))]

def get_examples_dirs():
    return [os.path.join(examples_dir, d) for d in get_examples_names()]

def read_example(example_dir):
    if not os.path.exists(example_dir):
        raise Exception(f"Example {example_dir} does not exist")

    if not os.path.isdir(example_dir):
        raise Exception(f"Example {example_dir} is not a directory")
    
    info_file = os.path.join(example_dir, 'info.json')
    if not os.path.exists(info_file):
        raise Exception(f"Example {example_dir} does not contain an info.json file")
    
    with open(info_file, 'r') as f:
        info = json.load(f)
        info['dir'] = example_dir
    
    return info

def build_example(example, clean=True, target="native", mode="debug"):
    build_dir = os.path.join(example['dir'], examples_build_dir_name)

    if target == "wasm":
        if not os.path.exists(wasm_cross_file):
            raise Exception(f"Cannot find cross file for wasm at {wasm_cross_file}.")
        
        if not os.path.exists(os.path.join(base_dir, "lib/lib/libremi.a")):
            raise Exception(f"Cannot find libremi.a in {base_dir}. Please build remi for wasm first.")

    if os.path.exists(build_dir) and clean:
        shutil.rmtree(build_dir)
    
    if not os.path.exists(build_dir):
        # create the build directory
        os.makedirs(build_dir)
        
        # copy the example directory to the build directory
        shutil.copytree(example['dir'], build_dir, dirs_exist_ok=True, ignore=shutil.ignore_patterns("info.json", examples_build_dir_name))

        # copy example build files to build dir
        shutil.copytree(examples_build_dir, build_dir, dirs_exist_ok=True)

        # copy example assets to build dir
        shutil.copytree(examples_helpers_assets_dir, os.path.join(build_dir, os.path.basename(examples_helpers_assets_dir)), dirs_exist_ok=True)
    
    # check if meson.build exists
    meson_build_file = os.path.join(build_dir, 'meson.build')
    if not os.path.exists(meson_build_file):
        raise Exception(f"Example {example['dir']} does not contain a meson.build file")
    
    # replace __SRC_FILES__ and __ASSETS_DIR__ in meson.build
    with open(meson_build_file, 'r') as f:
        meson_build_content = f.read()

        assets = []
        if example['assets_dir'] != "":
            assets.append(f"'{example['assets_dir']}'")
        
        if os.path.basename(examples_helpers_assets_dir) not in assets:
            assets.append(f"'{os.path.basename(examples_helpers_assets_dir)}'")

        meson_build_content = meson_build_content.replace("__ASSETS_DIR__", ", ".join(assets))
        meson_build_content = meson_build_content.replace("__SRC_FILES__", f"{example['src_files']}")
    
    with open(meson_build_file, 'w') as f:
        f.write(meson_build_content)

    # setup the example build if needed
    if not os.path.exists(os.path.join(build_dir, 'build')):
        if target == "wasm":
            exit_code, output = subprocess.getstatusoutput(f"cd {build_dir} && meson setup build --cross-file {wasm_cross_file} --buildtype {mode}")
        else:
            exit_code, output = subprocess.getstatusoutput(f"cd {build_dir} && meson setup build --buildtype {mode}")
        

        if exit_code != 0:
            raise Exception(f"Failed to setup the example build: {output}")
    
    # build the example
    exit_code, output = subprocess.getstatusoutput(f"cd {build_dir} && meson compile -C build")
    if exit_code != 0:
        raise Exception(f"Failed to build the example: {output}")

    # copy assets to build dir
    if example['assets_dir'] != "":
        if not os.path.exists(os.path.join(example['dir'], example['assets_dir'])):
            raise Exception(f"Example {example['dir']} does not contain a valid assets directory")

        shutil.copytree(os.path.join(example['dir'], example['assets_dir']), os.path.join(build_dir, 'build', example['assets_dir']), dirs_exist_ok=True)

    # copy example assets to meson build dir
    shutil.copytree(examples_helpers_assets_dir, os.path.join(build_dir, 'build', os.path.basename(examples_helpers_assets_dir)), dirs_exist_ok=True)
    
    # copy in remi dll
    if target == "native":
        remi_dll = os.path.join(base_dir, "lib/lib/libremi.dll")
        if not os.path.exists(remi_dll):
            raise Exception(f"Cannot find remi.dll in {remi_dll}.")
        
        shutil.copy(remi_dll, os.path.join(build_dir, 'build'))

    return build_dir

def run_example(example):
    build_dir = os.path.join(example['dir'], examples_build_dir_name)
    meson_build_dir = os.path.join(build_dir, 'build')

    if not os.path.exists(meson_build_dir):
        raise Exception(f"Example {example['dir']} has not been built")
    
    # run the example
    if os.path.exists(os.path.join(meson_build_dir, "example.html")):
        if os.path.exists(os.path.join(meson_build_dir, "index.html")):
            os.remove(os.path.join(meson_build_dir, "index.html"))
            
        os.rename(os.path.join(meson_build_dir, "example.html"), os.path.join(meson_build_dir, "example.html"))

        os.system(f"start http://localhost:8000/example.html")
        os.system(f"cd {meson_build_dir} && python -m http.server 8000")
    elif os.path.exists(os.path.join(meson_build_dir, "example.exe")):
        os.system(f"cd {meson_build_dir} && example.exe")
    else:
        os.system(f"cd {meson_build_dir} && example")


def run_and_build_example(example, clean=True, target="native", mode="debug"):
    print(f"{example['name']}: {example['description']}")

    print("Building...")
    build_example(example, clean, target, mode)

    print("Running...")
    run_example(example)

    
# parse arguments
parser = argparse.ArgumentParser("example")
parser.add_argument("example", help="Choose the example to build and run", choices=get_examples_names())
parser.add_argument("--clean", help="Clean the build directory before building the example", choices=["true", "false"], default="true")
parser.add_argument("--target", help="The target to build the example for", choices=["wasm", "native"], default="native")
parser.add_argument("--mode", help="The build mode", choices=["debug", "release"], default="release")

args = parser.parse_args()

example = read_example(os.path.join(examples_dir, args.example))
clean = args.clean == "true"
target = args.target
mode = args.mode

run_and_build_example(example, clean, target, mode)