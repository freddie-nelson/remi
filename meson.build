project('blaze++', 'cpp', default_options : ['libdir=' + meson.current_source_dir() / 'lib' / 'lib', 'includedir=' + meson.current_source_dir() / 'lib' / 'include'])
add_project_arguments('-std=c++20', language : 'cpp')

incdir = include_directories('externals/include')
dependencies = [dependency('glfw3'), dependency('glm'), dependency('boost')]

lib_dir = meson.current_source_dir() / 'lib'

link_args = []

#
# glad
#

glad_src = ['externals/src/glad/gl.cpp']
glad = library('glad', glad_src, include_directories : incdir, install : true, install_dir : lib_dir / 'lib', link_args : link_args)

install_subdir('externals/include/glad', install_dir : lib_dir / 'include/glad', strip_directory : true)
install_subdir('externals/src/glad', install_dir : lib_dir / 'src/glad', strip_directory : true)

#
# blaze
#

# core
core_src = ['src/Core/BoundingCircle.cpp', 'src/Core/Transform.cpp', 'src/Core/Timestep.cpp', 'src/Core/Window.cpp']
core_src += ['src/Core/AABB/AABB.cpp']

# ecs
ecs_src = ['src/ECS/Entity.cpp', 'src/ECS/Registry.cpp']

# input
input_src = ['src/Input/Mouse.cpp']

# rendering
rendering_src = ['src/Rendering/Renderer.cpp'] 
rendering_src += ['src/Rendering/Camera/Camera.cpp']
rendering_src += ['src/Rendering/Material/Color.cpp', 'src/Rendering/Material/Material.cpp']
rendering_src += ['src/Rendering/Mesh/Mesh.cpp', 'src/Rendering/Mesh/Polygons.cpp', 'src/Rendering/Mesh/Triangulate.cpp']
rendering_src += ['src/Rendering/Shader/Shader.cpp']
rendering_src += ['src/Rendering/Texture/Texture.cpp', 'src/Rendering/Texture/TextureAtlas.cpp', 'src/Rendering/Texture/TextureManager.cpp']
rendering_src += ['src/Rendering/Utility/FileHandling.cpp', 'src/Rendering/Utility/OpenGLHelpers.cpp']

engine_src = ['src/Engine.cpp']

all_src = core_src + ecs_src + input_src + rendering_src + engine_src

engine = library('blaze++', all_src, dependencies : dependencies, include_directories : incdir, link_with : [glad], install : true, install_dir : lib_dir / 'lib', link_args : link_args)

# install src and headers
install_subdir('src', install_dir : lib_dir / 'src/blaze++', strip_directory : true)
install_subdir('include', install_dir : lib_dir / 'include/blaze++', strip_directory : true)

# install pkgconfig

pkg = import('pkgconfig')
pkg.generate(engine, version : '1.0', name : 'blaze++', filebase : 'blaze++', description : 'The blaze game engine.', install_dir : lib_dir / 'lib/pkgconfig', libraries : [glad])