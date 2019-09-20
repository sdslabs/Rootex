git submodule init
git submodule update
cd vcpkg
if not exist vcpkg.exe ./bootstrap-vcpkg.bat
vcpkg.exe install glew:x64-windows
vcpkg.exe install glfw3:x64-windows
cd ..
mkdir Build
cd Build
cmake .. "-DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake" -G "Visual Studio 15 2017 Win64"
start Rootex.sln
cd ..
