mkdir build
cd build
del /F CMakeCache.txt
cmake .. -G "Visual Studio 16 2019" -A x64
cd ..
