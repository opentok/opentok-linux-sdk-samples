# Meet Publisher Only

The Meet Publisher Only sample application is a very simple application meant to
get a new developer started using the OpenTok Linux SDK.

Upon building and running this sample application, you should be able to have
one way audio and video communication using OpenTok. This sample application is
intended to be used in a tiny single-board computer such as the Raspberry Pi.

The new thing here is that in this sample application you are joining a Meet
session at https://meet.tokbox.com (https://github.com/opentok/opentok-meet).

## Building and running the sample app

Before building the sample application you have to uncompress the OpenTok Linux
SDK package in the [assets](../assets) folder.

Uncompress the one for the target host you are building the sample application
for. We would uncompress
[assets/opentok-ot-libotkit-ubuntu_armv7-2.17.0-preview.0.tgz](../assets/opentok-ot-libotkit-ubuntu_armv7-2.17.0-preview.0.tgz)]
for Ubuntu armv7.

```
$ cd assets; unp opentok-ot-libotkit-ubuntu_armv7-2.17.0-preview.0.tgz
```

This will create a `package` folder at [assets/package](../assets/package). The
sample application build bits will try find the OpenTok Linux SDK header files
and library under the `package` folder.

We use [CMake](https://cmake.org) in this sample application so you have to
install it before trying to build the sample application. There are other
dependencies you have to install too. These are:
  - build-essential
  - cmake
  - clang
  - libc++-dev
  - libc++abi-dev
  - pkg-config
  - libuv1-dev
  - libunwind-dev
  - libcurl4-openssl-dev
  - libjansson-dev
  - libasound2
  - libpulse-dev
  - libsdl2-dev

Let's build the sample application. It is a good practice to create a build
folder. Let's create one.

```
$ mkdir Meet-Publisher-Only/build
```

Next step is to create the building bits using `cmake`.

```
$ cd Meet-Publisher-Only/build
$ CC=clang CXX=clang++ cmake ..
```

Note we are using `clang/clang++` compilers here.

Next step is building the code using `make`.

```
$ make
```

Once it finishes the `meet_publisher_only` binary is ready so just run it. In this
sample application you have to provide the Meet room you are joinning to.

```
$ ./publisher_only --room linuxsdk
```

Open https://meet.tokbox.com/linux_sdk in your browser and you will see the
audio and video string coming from the sample application running on Linux.

End the sample application by sending a SIGBREAK (21) signal to the sample
application in the console.

```
$ kill -21 <meet_publisher_only PID>
```
