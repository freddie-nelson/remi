project('dev', 'cpp')
add_project_arguments('-std=c++20', language : 'cpp')

fs = import('fs')

cross_target = meson.get_external_property('cross_target', 'native')

cpp_compiler = meson.get_compiler('cpp')

#
# configure for cross compilation
#

inc_dirs = []
dependencies = []
cpp_args = ['-I' + join_paths(meson.current_source_dir(), '../externals/include')]
link_args = ['-I' + join_paths(meson.current_source_dir(), '../externals/include')]
link_with = []
name_suffix = ''

if cross_target == 'native'
    dependencies += [dependency('blaze++', required : true)]
    name_suffix = 'exe'
endif

if cross_target == 'wasm'
    lib_dir = join_paths(meson.current_source_dir(), '../lib')

    includes = ['-I' + join_paths(lib_dir, 'include')]
    link_args += includes
    cpp_args += includes 

    link_args += ['-sUSE_GLFW=3', '-sMIN_WEBGL_VERSION=2', '-sMAX_WEBGL_VERSION=2', '-sALLOW_MEMORY_GROWTH=1']
    if get_option('buildtype') == 'debug'
        link_args += ['-sASSERTIONS=1', '-sNO_DISABLE_EXCEPTION_CATCHING']

        # crashing on wasm with SAFE_HEAP when trying to load a font
        # link_args += ['-sSAFE_HEAP=1',]
    endif

    # link_args += ['--embed-file ' + join_paths(meson.current_source_dir(), 'assets')]

    # preload assets
    preload = ['assets'] # relative to the meson.build file directory

    foreach p : preload
        link_args += ['--preload-file', join_paths(meson.current_source_dir(), p) + '@/' + p]
    endforeach

    # must have built blaze for wasm previously
    blaze = cpp_compiler.find_library('blaze++', dirs : join_paths(lib_dir, 'lib'), required : true) 
    ttf2mesh = cpp_compiler.find_library('ttf2mesh', dirs : join_paths(lib_dir, 'lib'), required : true)

    dependencies += [blaze, ttf2mesh]

    name_suffix = 'html'
endif

src = ['main.cpp', 'src/Application.cpp', 'src/Globals.cpp']
executable('dev', src, dependencies : dependencies, link_args : link_args, cpp_args : cpp_args, link_with : link_with, name_suffix : name_suffix, include_directories : inc_dirs)