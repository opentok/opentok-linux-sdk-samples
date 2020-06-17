FROM debian:buster

RUN echo "deb http://security.debian.org/ buster/updates main" | tee -a /etc/apt/sources.list

RUN apt-get update \
    && apt-get -yqqf --no-install-recommends install \
       ca-certificates \
       wget \
       gnupg \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

RUN echo "deb https://dl.bintray.com/tokbox/debian buster main" | tee -a /etc/apt/sources.list
RUN wget -O- -q https://bintray.com/user/downloadSubjectPublicKey?username=bintray | apt-key add -

RUN apt-get update \
  && apt-get install -y \
     software-properties-common \
     build-essential \
     cmake \
     clang \
     libc++-dev \
     libc++abi-dev \
     libsdl2-dev \
     libopentok-dev \
  && apt-get clean \
  && rm -rf /var/lib/apt/lists/*

VOLUME /samples
WORKDIR /samples/scripts

CMD bash -x build.sh
