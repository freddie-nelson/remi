make

rmdir build /Q /S

mkdir build
cd build
mkdir assets
cd ..

move main.exe build
copy lib build
copy assets build\assets