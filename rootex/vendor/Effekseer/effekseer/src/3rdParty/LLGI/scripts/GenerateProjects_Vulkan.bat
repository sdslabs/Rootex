echo set current directory
cd /d %~dp0

mkdir ..\build

cd /d ..\build

cmake -D BUILD_TEST=ON -D BUILD_VULKAN=ON -D BUILD_VULKAN_COMPILER=ON -D BUILD_EXAMPLE=ON -D BUILD_TOOL=ON ../

pause
