#!/bin/sh

echo set current directory
cd `dirname $0`

mkdir ../build_clangformat

cd ../build_clangformat

cmake -D BUILD_TEST=ON -D BUILD_EXAMPLE=ON -DCLANG_FORMAT_ENABLED=ON ../
