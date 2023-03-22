FROM i386/ubuntu:20.04

RUN apt-get -y update && apt-get -y install build-essential libx11-dev libxfixes-dev && rm -rf /var/lib/apt/lists/*

WORKDIR /

COPY makefile /


CMD ["make"]
#ENTRYPOINT "make"
