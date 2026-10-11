FROM i386/ubuntu:20.04
#FROM i386/ubuntu

ARG TZ=Europe/London
ENV TZ $TZ
RUN apt-get -y update && ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone && DEBIAN_FRONTEND="noninteractive" apt-get -y install tzdata git build-essential libx11-dev libxfixes-dev libcurl4-openssl-dev pkg-config zlib1g-dev libssl-dev libpython3-dev && rm -rf /var/lib/apt/lists/*

# ===============================================================
# Compile cmake
WORKDIR /

RUN git clone https://github.com/Kitware/CMake

WORKDIR CMake

RUN ./bootstrap -- -DCMAKE_USE_OPENSSL=OFF && make && make install

# ===============================================================
# Compile minizip

WORKDIR /

RUN git clone https://github.com/zlib-ng/minizip-ng minizip

WORKDIR /minizip

RUN cmake . -B build -DCMAKE_SYSTEM_NAME=Linux -DCMAKE_C_FLAGS="-m32 -msse4.1" -DCMAKE_CXX_FLAGS="-m32 -msse4.1" -DCMAKE_ASM_FLAGS=-m32 -DMZ_FETCH_LIBS=ON -DMZ_ZLIB=ON

RUN cmake --build build

# install the minizip libraries
RUN mkdir -p /usr/include/minizip
RUN cp /minizip/build/*.h /usr/include/minizip/ || true
RUN cp /minizip/*.h /usr/include/minizip/ || true
RUN cp /minizip/build/libminizip.a /lib/ || true
RUN cp /minizip/compat/*.h /usr/include/minizip/ || true


# ===============================================================
# Compile sof-plus-plus-nix
WORKDIR /src

# COPY makefile /

RUN apt-get -y update && apt-get -y install libsdl1.2-dev libbsd-dev libpython3-dev python3 python3-dev && rm -rf /var/lib/apt/lists/*

CMD ["make"]
#ENTRYPOINT "make"
