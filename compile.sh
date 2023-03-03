#!/bin/bash

gcc -m32 -shared -fpic src/main.cpp -o test.so
./install.sh
