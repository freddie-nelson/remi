
if "%~1"=="" (
    echo "No argument given, use either 'sfml' or 'sdl'"
    exit
)

if not %1% == sfml (
    if not %1% == sdl (
        echo "Invalid argument, use either 'sfml' or 'sdl'"
        exit
    )
)

if %1% == sfml (
@REM compile without sdl
move src\Rendering\SDLRenderer.cpp .\
move src\Utility\SDLHelpers.cpp .\
make -f makefile-sfml
move SDLRenderer.cpp src\Rendering\
move SDLHelpers.cpp src\Utility\
) 

if %1% == sdl (
@REM compile without sfml
move src\Rendering\SFMLRenderer.cpp .\
make -f makefile-sdl
move SFMLRenderer.cpp src\Rendering\
) 

rmdir build /Q /S

mkdir build
cd build
mkdir assets
cd ..

move main.exe build
copy lib build
copy assets build\assets