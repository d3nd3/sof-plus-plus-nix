#!/bin/bash
docker create --name temp-libpython3-builder libpython3-builder
docker cp temp-libpython3-builder:/app/Python-3.8.2/libpython3.8.a ./libpython3.8.a
mkdir -p lib/
docker cp temp-libpython3-builder:/app/Python-3.8.2/Lib lib/python3.8
docker rm temp-libpython3-builder
