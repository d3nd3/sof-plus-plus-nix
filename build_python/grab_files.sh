#!/bin/bash
docker create --name temp-libpython3-builder libpython3-builder
docker cp temp-libpython3-builder:/app/Python-3.9.2/libpython3.9.a ./libpython3.9.a
docker rm temp-libpython3-builder
