project('dev', 'cpp')
add_project_arguments('-std=c++20', language : 'cpp')

dependencies = [dependency('blaze++', required : true)]

src = ['main.cpp', 'src/Application.cpp', 'src/Globals.cpp']
executable('dev', src, dependencies : dependencies, link_args : [])