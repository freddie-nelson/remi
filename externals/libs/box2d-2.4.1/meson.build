box2d_incdir = include_directories('include')
box2d_compile_args = []
if get_option('buildtype') != 'static'
  box2d_compile_args += '-DB2_SHARED'
  box2d_compile_args += '-Dbox2d_EXPORTS'
endif

subdir('src')
