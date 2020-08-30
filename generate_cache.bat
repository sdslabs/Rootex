mkdir build
cd build
del /F CMakeCache.txt
cmake .. -DBUILD_EDITOR=ON -G "Visual Studio 15 2017" -A x64
cd ..
