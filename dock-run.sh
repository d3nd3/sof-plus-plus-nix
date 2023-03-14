#!/bin/bash
echo $PWD
docker run --rm -v $PWD/src:/src -v $PWD/out:/out compiler
