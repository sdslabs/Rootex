git submodule init
git submodule update
cd vcpkg
if not exist vcpkg.exe ./bootstrap-vcpkg.bat
vcpkg.exe update
vcpkg.exe upgrade
vcpkg.exe install lua:x64-windows
vcpkg.exe integrate install
cd ..
mkdir Build
cd Build
cmake .. "-DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake" -G "Visual Studio 15 2017 Win64"
cd ..
