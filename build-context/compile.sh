#!/bin/bash
# -D _GNU_SOURCE for RTLD_NEXT
# -D WSL ( Clipboard WSL )
gcc -m32 -shared -fpic -fpermissive -w -D _GNU_SOURCE ../src/main.c -o out/test.so -lX11
