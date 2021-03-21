#!/bin/sh

echo set current directory
cd `dirname $0`

mkdir ../build

cd ../build

cmake -D BUILD_TEST=ON -D BUILD_EXAMPLE=ON -G "Xcode" ../
