#FROM i386/ubuntu:22.04
FROM i386/ubuntu

ARG TZ=Europe/London
ENV TZ $TZ
RUN apt-get -y update && ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone && DEBIAN_FRONTEND="noninteractive" apt-get -y install tzdata git build-essential libx11-dev libxfixes-dev libcurl4-openssl-dev pkg-config zlib1g-dev libssl-dev libpython3-dev && rm -rf /var/lib/apt/lists/*

WORKDIR /

RUN git clone https://github.com/Kitware/CMake

WORKDIR CMake

RUN ./bootstrap && make && make install

WORKDIR /

RUN git clone https://github.com/zlib-ng/minizip-ng minizip

WORKDIR /minizip

RUN cmake . -B build -DCMAKE_SYSTEM_NAME=Linux -DCMAKE_C_FLAGS="-m32 -msse4.1" -DCMAKE_CXX_FLAGS="-m32 -msse4.1" -DCMAKE_ASM_FLAGS=-m32 -DMZ_FETCH_LIBS=ON -DMZ_ZLIB=ON

RUN cmake --build build

RUN mkdir -p /usr/include/minizip
RUN cp /minizip/build/*.h /usr/include/minizip/
RUN cp /minizip/*.h /usr/include/minizip/
RUN cp /minizip/build/libminizip.a /lib/

WORKDIR /src

# COPY makefile /

RUN apt-get -y update && apt-get -y install libbsd-dev libpython3.9-dev libpython3.9 python3.9 python3.9-dev && rm -rf /var/lib/apt/lists/*

CMD ["make"]
#ENTRYPOINT "make"