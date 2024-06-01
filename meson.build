project('remi', 'cpp', 'c', default_options : ['libdir=' + meson.current_source_dir() / 'lib' / 'lib', 'includedir=' + meson.current_source_dir() / 'lib' / 'include'])
add_project_arguments('-std=c++20', language : 'cpp')

cross_target = meson.get_external_property('cross_target', 'native')

install_libs = true
# if cross_target == 'native'
#     install_libs = true
# endif

install_headers = true

incdir = include_directories('externals/include')
lib_dir = meson.current_source_dir() / 'lib'

wasm_link_args = ['-sEXPORT_ALL=1', '-sALLOW_MEMORY_GROWTH=1', '-sMAXIMUM_MEMORY=4096MB', '-sINVOKE_RUN=0', '-sFORCE_FILESYTEM', '-sSTANDALONE_WASM']

if get_option('buildtype') == 'debug'
    wasm_link_args += ['-sASSERTIONS=2', '-sNO_DISABLE_EXCEPTION_CATCHING', '-gsource-map', '-g3', '-O0', '-sDEMANGLE_SUPPORT=1']

    # crashing on wasm when trying to load a font
    # wasm_link_args += ['-sSAFE_HEAP=1'] 
endif

# if get_option('buildtype') == 'release'
#     add_project_arguments('-g', language : 'cpp')
# endif

#
# glad
#

glad_src = ['externals/src/glad/glad.c']
glad_kwargs = {
    'include_directories' : incdir,
    'install' : install_libs,
    'install_dir' : lib_dir / 'lib',
}

if cross_target == 'native'
    glad = static_library('glad', glad_src, link_args : [], kwargs : glad_kwargs)
    
    if install_headers
        install_subdir('externals/include/glad', install_dir : lib_dir / 'include/glad', strip_directory : true)
        install_subdir('externals/src/glad', install_dir : lib_dir / 'src/glad', strip_directory : true)
    endif
endif

#
# ttf2mesh
#

ttf2mesh_src = ['externals/src/ttf2mesh.c']
ttf2mesh_kwargs = {
    'include_directories' : incdir,
    'install' : install_libs,
    'install_dir' : lib_dir / 'lib',
}

if cross_target == 'native'
    ttf2mesh = static_library('ttf2mesh', ttf2mesh_src, link_args : [], kwargs : ttf2mesh_kwargs)
endif

if cross_target == 'wasm'
    ttf2mesh = static_library('ttf2mesh', ttf2mesh_src, link_args : wasm_link_args, kwargs : ttf2mesh_kwargs)
endif

if install_headers
    install_data('externals/include/ttf2mesh.h', install_dir : lib_dir / 'include')
    install_data('externals/src/ttf2mesh.c', install_dir : lib_dir / 'src')
endif

#
# box2d
#

# produces box2d_lib
subdir('externals/libs/box2d-2.4.1')

if install_headers
    install_subdir('externals/include/box2d', install_dir : lib_dir / 'include/box2d', strip_directory : true)
endif

#
# configure for cross compilation
#

dependencies = []
cpp_args = []
link_args = []
link_with = []

if cross_target == 'native'
    dependencies += [dependency('sdl2', required : true, static : true), dependency('sdl2_mixer', required : true, static : true) dependency('boost', required: true, static : true)]
    link_with += [glad, ttf2mesh, box2d_lib]
endif

if cross_target == 'wasm'
    link_args += ['-sUSE_SDL=2', '-sMIN_WEBGL_VERSION=2', '-sMAX_WEBGL_VERSION=2'] + wasm_link_args

    if get_option('buildtype') == 'debug'
        link_args += ['-sASSERTIONS=1', '-sNO_DISABLE_EXCEPTION_CATCHING']
    endif
endif

#
# remi
#

# core
core_src = ['src/Core/BoundingCircle.cpp', 'src/Core/Transform.cpp', 'src/Core/Timestep.cpp', 'src/Core/Window.cpp', 'src/Core/SpaceTransformer.cpp']
core_src += ['src/Core/AABB/AABB.cpp']

# debug
# debug_src = ['src/Debug/DebugInfo.cpp']

# ecs
ecs_src = ['src/ECS/Registry.cpp']

# input
input_src = ['src/Input/Mouse.cpp', 'src/Input/Keyboard.cpp']

# physics
physics_src = ['src/Physics/PhysicsWorld.cpp', 'src/Physics/RigidBody2D.cpp', 'src/Physics/Collider2D.cpp', 'src/Physics/ColliderShape.cpp', 'src/Physics/Ray.cpp', 'src/Physics/QueryCallback.cpp', 'src/Physics/ContactListener.cpp', 'src/Physics/MouseJointUpdateSystem.cpp', 'src/Physics/ContactFilter.cpp']
physics_src += [
    'src/Physics/Joints/Joint.cpp',
    'src/Physics/Joints/DistanceJoint.cpp',
    'src/Physics/Joints/RevoluteJoint.cpp',
    'src/Physics/Joints/PrismaticJoint.cpp',
    'src/Physics/Joints/PulleyJoint.cpp',
    'src/Physics/Joints/GearJoint.cpp',
    'src/Physics/Joints/MouseJoint.cpp',
    'src/Physics/Joints/WheelJoint.cpp',
    'src/Physics/Joints/WeldJoint.cpp',
    'src/Physics/Joints/MotorJoint.cpp',
    'src/Physics/Joints/FrictionJoint.cpp',
]

# rendering
rendering_src = ['src/Rendering/Renderer.cpp', 'src/Rendering/RenderTarget.cpp', 'src/Rendering/RenderPipeline.cpp', 'src/Rendering/RenderManager.cpp', 'src/Rendering/Renderable.cpp'] 
rendering_src += ['src/Rendering/Camera/Camera.cpp']
rendering_src += ['src/Rendering/Font/Font.cpp', 'src/Rendering/Font/Text.cpp', 'src/Rendering/Font/MemoizedText.cpp']
rendering_src += ['src/Rendering/Material/Color.cpp', 'src/Rendering/Material/Material.cpp', 'src/Rendering/Material/ShaderMaterial.cpp', 'src/Rendering/Material/AnimatedMaterial.cpp', 'src/Rendering/Material/MaterialHelpers.cpp']
rendering_src += ['src/Rendering/Mesh/Mesh.cpp', 'src/Rendering/Mesh/Polygons.cpp', 'src/Rendering/Mesh/Triangulate.cpp']
rendering_src += [
    'src/Rendering/Passes/BatchPass.cpp', 
    'src/Rendering/Passes/BrightnessPass.cpp',
    'src/Rendering/Passes/ColorBlendPass.cpp', 
    'src/Rendering/Passes/CullingPass.cpp',
    'src/Rendering/Passes/DrawPass.cpp',
    'src/Rendering/Passes/GaussianBlurPass.cpp',
    'src/Rendering/Passes/OutputPass.cpp', 
    'src/Rendering/Passes/PhysicsDebugPass.cpp',
    'src/Rendering/Passes/PosterizePass.cpp',
    'src/Rendering/Passes/PostProcessingPass.cpp', 
    'src/Rendering/Passes/RenderablesPass.cpp', 
    'src/Rendering/Passes/DebugRenderTreePass.cpp'
]
rendering_src += ['src/Rendering/Shader/Shader.cpp', 'src/Rendering/Shader/VertexIndices.cpp']
rendering_src += ['src/Rendering/Texture/AnimatedTexture.cpp', 'src/Rendering/Texture/AnimationSystem.cpp', 'src/Rendering/Texture/Texture.cpp', 'src/Rendering/Texture/TextureAtlas.cpp', 'src/Rendering/Texture/TextureManager.cpp']
rendering_src += ['src/Rendering/Utility/OpenGLHelpers.cpp']

# scene
scene_src = ['src/Scene/SceneGraph.cpp']

# utility
utility_src = ['src/Utility/FileHandling.cpp', 'src/Utility/TypeHelpers.cpp', 'src/Utility/SDLHelpers.cpp']

# world
world_src = ['src/World/World.cpp']

# engine
engine_src = ['src/Engine.cpp', 'src/emscriptenHelpers.cpp']

all_src = core_src + ecs_src + input_src + physics_src + rendering_src + scene_src + utility_src + world_src + engine_src

# add lib objects for wasm
lib_objects = []

if cross_target == 'wasm'
    lib_objects += [box2d_lib.extract_all_objects(), ttf2mesh.extract_all_objects()]
endif

# create args
engine_kwargs = {
    'dependencies' : dependencies,
    'include_directories' : incdir,
    'link_with' : link_with,
    'install' : install_libs,
    'install_dir' : lib_dir / 'lib',
    'link_args' : link_args,
    'cpp_args' : cpp_args,
    'objects' : lib_objects,
}

if cross_target == 'native'
    engine = library('remi', all_src, kwargs : engine_kwargs)
endif

if cross_target == 'wasm'

    engine = static_library('remi', all_src, kwargs : engine_kwargs)
endif

if install_libs
    # install pkgconfig
    # pkg = import('pkgconfig')
    # pkg.generate(engine, version : '1.0', name : 'remi', filebase : 'remi', description : 'The remi game engine.', install_dir : lib_dir / 'lib/pkgconfig', libraries : [glad])
endif

if install_headers
    # install src and headers
    install_subdir('src', install_dir : lib_dir / 'src/remi', strip_directory : true)
    install_subdir('include', install_dir : lib_dir / 'include/remi', strip_directory : true)

    # install externals
    install_subdir('externals/include', install_dir : lib_dir / 'include', strip_directory : true, exclude_directories : ['boost'])
endif
