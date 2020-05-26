#!/bin/bash

sudo apt-get update
sudo apt-get install -y build-essential \
                        cmake \
                        clang \
                        libc++-dev \
                        libc++abi-dev \
                        pkg-config \
                        libuv1-dev \
                        libunwind-dev \
                        libcurl4-openssl-dev \
                        libjansson-dev \
                        libasound2 \
                        libpulse-dev \
                        libsdl2-dev
echo "Install Debian packages from https://lumbergh.tokbox.com/artifacts/trunk/opentok-linux-sdk until we have our own Debian repository."
