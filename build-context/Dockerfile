FROM i386/ubuntu:16.04

RUN apt-get -y update && apt-get -y install build-essential libx11-dev libxfixes-dev && rm -rf /var/lib/apt/lists/*

WORKDIR /

COPY compile.sh /


#CMD ["echo", "override me"]
ENTRYPOINT "./compile.sh"
