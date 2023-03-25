#!/bin/bash
echo $PWD
docker run --rm -it -v $PWD/src:/src -v $PWD/obj:/obj -v $PWD/hdr:/hdr -v $PWD/out:/out  -v $HOME/.loki/sof-runtime:/root/.loki/sof-runtime compiler /bin/bash
