echo set current directory
cd /d %~dp0

mkdir ..\build_clangformat

cd /d ..\build_clangformat

cmake -A x64 -D BUILD_TEST=ON -D BUILD_EXAMPLE=ON -D BUILD_TOOL=ON -DCLANG_FORMAT_ENABLED=ON ../

pause
