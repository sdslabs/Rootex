git submodule init
git submodule update
cd vcpkg
vcpkg_bin=vcpkg
if test -f "vcpkg"; then
    echo "vcpkg exists"
else
	echo "Running bootstrap..."
fi
./bootstrap-vcpkg.sh
./vcpkg install glew:x64-linux
./vcpkg install glfw3:x64-linux
cd ..
mkdir Build
cd Build
cmake .. "-DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake"
make
cd ..
