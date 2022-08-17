@echo off
set res=F
if "%1"=="/22" set res=T
if "%1"=="/19" set res=T
if "%1"=="/17" set res=T
if "%res%"=="T" (
@echo on
    mkdir build
    cd build
    del /F CMakeCache.txt
    if "%1"=="/22" (
        cmake .. -G "Visual Studio 17 2022" -A x64
    )
    if "%1"=="/19" (
        cmake .. -G "Visual Studio 16 2019" -A x64
    )
    if "%1"=="/17" (
        cmake .. -G "Visual Studio 15 2017" -A x64
    )
    cd ..
) else (
    echo VS Version not specified
    exit 1
)