FROM debian:buster

RUN echo "deb http://security.debian.org/ buster/updates main" | tee -a /etc/apt/sources.list

RUN apt-get update \
    && apt-get -yqqf --no-install-recommends install \
       ca-certificates \
       wget \
       gnupg \
       curl \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

RUN curl -s https://packagecloud.io/install/repositories/tokbox/debian/script.deb.sh | bash

RUN apt-get update \
  && apt-get install -y \
     software-properties-common \
     build-essential \
     cmake \
     clang \
     libc++-dev \
     libc++abi-dev \
     libsdl2-dev \
     libopencv-dev \
     libopentok-dev \
  && apt-get clean \
  && rm -rf /var/lib/apt/lists/*

VOLUME /samples
WORKDIR /samples/scripts

CMD bash -x build.sh
