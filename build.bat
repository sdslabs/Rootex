@echo off
IF "%1"=="" (
    cd build/
    cmake --build .
)
