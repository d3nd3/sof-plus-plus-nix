# Use i386/ubuntu:20.04 as the base image
FROM i386/ubuntu:20.04

# Set the working directory
WORKDIR /app

# Install necessary build dependencies
ARG TZ=Europe/London
ENV TZ $TZ
RUN apt-get -y update && ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone && DEBIAN_FRONTEND="noninteractive" apt-get install -y \
    build-essential \
    wget \
    zlib1g-dev \
    libssl-dev \
    libbz2-dev \
    libreadline-dev \
    libsqlite3-dev \
    libffi-dev \
    libncurses5-dev \
    libgdbm-dev \
    libnss3-dev

# Download Python source code
RUN wget https://www.python.org/ftp/python/3.8.2/Python-3.8.2.tgz \
    && tar -xvf Python-3.8.2.tgz

# Copy Setup file from the context to Modules/Setup
COPY Setup /app/Python-3.8.2/Modules/Setup

# Change into Python source code directory
WORKDIR /app/Python-3.8.2

# Configure Python build
# --enable-unicode=ucs4
# --enable-optimizations
RUN ./configure --disable-shared --enable-optimizations LDFLAGS="-static"

# Compile Python
RUN make -j 4 LDFLAGS="-static" LINKFORSHARED=" "

# Set the entrypoint as a placeholder
ENTRYPOINT ["/bin/bash"]
