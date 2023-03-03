#!/bin/bash
echo $PWD
docker run --rm -v $PWD/src:/src -v $PWD/out:/out compiler gcc -m32 -shared -fpic src/main.cpp -o out/test.so
