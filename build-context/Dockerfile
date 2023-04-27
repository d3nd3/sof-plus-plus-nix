FROM i386/ubuntu:20.04

ARG TZ=Europe/London
ENV TZ $TZ
RUN apt-get -y update && ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone && DEBIAN_FRONTEND="noninteractive" apt-get -y install tzdata git build-essential cmake libx11-dev libxfixes-dev libcurl4-openssl-dev pkg-config zlib1g-dev libssl-dev && rm -rf /var/lib/apt/lists/*

WORKDIR /

RUN git clone https://github.com/zlib-ng/minizip-ng minizip

WORKDIR /minizip

RUN cmake -DCMAKE_SYSTEM_NAME=Linux -DCMAKE_C_FLAGS=-m32 -DCMAKE_CXX_FLAGS=-m32 -S . -B build -D MZ_FETCH_LIBS=ON -D MZ_ZLIB=ON

RUN cmake --build build

RUN mkdir -p /usr/include/minizip
RUN cp /minizip/build/*.h /usr/include/minizip/
RUN cp /minizip/*.h /usr/include/minizip/
RUN cp /minizip/build/libminizip.a /lib/

WORKDIR /src

# COPY makefile /


CMD ["make"]
#ENTRYPOINT "make"