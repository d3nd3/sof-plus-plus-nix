#!/bin/bash
# -D _GNU_SOURCE for RTLD_NEXT

gcc -m32 -shared -fpic -fpermissive -w -D _GNU_SOURCE -D WSL src/main.c -o test.so -l X11
./install.sh
