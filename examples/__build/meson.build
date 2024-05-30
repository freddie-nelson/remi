project('example', 'cpp')
add_project_arguments('-std=c++20', language : 'cpp')

fs = import('fs')

cross_target = meson.get_external_property('cross_target', 'native')
lib_dir = join_paths(meson.current_source_dir(), '../../../lib')
helpers_dir = join_paths(meson.current_source_dir(), '../../__helpers')
externals_dir = join_paths(meson.current_source_dir(), '../../../externals/include')

cpp_compiler = meson.get_compiler('cpp')

if cross_target == 'native'
    remi = cpp_compiler.find_library('remi', dirs : join_paths(lib_dir, 'lib'), required : true, static : false) 
endif

if cross_target == 'wasm'
    remi = cpp_compiler.find_library('remi', dirs : join_paths(lib_dir, 'lib'), required : true, static : true)
endif

#
# configure for cross compilation
#

inc_dirs = []
dependencies = []
cpp_args = []
link_args = []
link_with = []
name_suffix = ''

# add remi to the dependencies
dependencies += [remi]

includes = ['-I' + join_paths(lib_dir, 'include'), '-I' + helpers_dir, '-I' + externals_dir]
link_args += includes
cpp_args += includes 

# native build
if cross_target == 'native'
    name_suffix = 'exe'

    if get_option('buildtype') == 'release'
        link_args += ['-mwindows']
    endif
endif

# wasm build
if cross_target == 'wasm'
    link_args += ['-sUSE_SDL=2', '-sMIN_WEBGL_VERSION=2', '-sMAX_WEBGL_VERSION=2', '-sALLOW_MEMORY_GROWTH=1', '-sMAXIMUM_MEMORY=4096MB']
    
    if get_option('buildtype') == 'debug'
        link_args += ['-sASSERTIONS=2', '-sNO_DISABLE_EXCEPTION_CATCHING', '-gsource-map', '-g3', '-O0', '-sDEMANGLE_SUPPORT=1']

        # crashing on wasm with SAFE_HEAP when trying to load a font
        # link_args += ['-sSAFE_HEAP=1',]

        # link_args += ['-fsanitize=address']
    endif

    # shell file
    link_args += ['--shell-file', join_paths(meson.current_source_dir(), 'em-shell.html')]

    # preload assets
    preload = [__ASSETS_DIR__] # relative to the meson.build file directory

    foreach p : preload
        link_args += ['--preload-file', join_paths(meson.current_source_dir(), p) + '@/' + p]
    endforeach

    name_suffix = 'html'
endif

src = __SRC_FILES__
executable('example', src, dependencies : dependencies, link_args : link_args, cpp_args : cpp_args, link_with : link_with, name_suffix : name_suffix, include_directories : inc_dirs)