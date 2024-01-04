import os

dir_path = os.path.dirname(os.path.realpath(__file__))

lib_path = os.path.join(dir_path, 'lib')
libdir = os.path.join(lib_path, 'lib')
includedir = os.path.join(lib_path, 'include')

options = []

# options.append({
#     'name': 'lib',
#     'type': 'string',
#     'value': lib_path,
#     'description': 'library installation path'
# })

# options.append({
#     'name': 'libdir',
#     'type': 'string',
#     'value': libdir,
# })

# options.append({
#     'name': 'includedir',
#     'type': 'string',
#     'value': includedir,
# })

# write options
filename = 'meson.options'
contents = ''

for option in options:
    contents += f'option(\'{option["name"]}\', type : \'{option["type"]}\', value : \'{option["value"]}\', description : \'{ option["description"] if "description" in option else ""}\')\n'

with open(filename, 'w') as f:
    f.write(contents)

        
