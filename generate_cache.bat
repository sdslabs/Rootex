mkdir build
cd build
del /F CMakeCache.txt
cmake .. -DBUILD_EDITOR=ON -G "Visual Studio 16 2019" -A x64
cd ..
