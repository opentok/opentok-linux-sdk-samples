#!/bin/bash

echo "deb https://dl.bintray.com/tokbox/debian buster main" | sudo tee -a /etc/apt/sources.list
wget -O- -q https://bintray.com/user/downloadSubjectPublicKey?username=bintray | sudo apt-key add -
sudo apt-get update
sudo apt-get install -y build-essential \
                        cmake \
                        clang \
                        libc++-dev \
                        libc++abi-dev \
                        pkg-config \
                        libasound2 \
                        libpulse-dev \
                        libsdl2-dev \
                        libopentok-dev
