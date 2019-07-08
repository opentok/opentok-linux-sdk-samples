# OpenTokDemo Basic Video Chat

The Basic Video Chat app is a very simple application meant to get a new
developer started using the OpenTok Linux SDK.

Upon building and running this sample application, you should be able to have
two way audio and video communication using OpenTok.

The new thing here is that in this sample application you are joining an
OpenTokDemo session at https://opentokdemo.tokbox.com.

## Building and running the sample app

Before building the sample application you have to uncompress the OpenTok Linux
SDK package in the [assets](../assets) folder.

Uncompress the one for the target host you are building the sample application
for. We would uncompress
[assets/opentok-ot-libotkit-ubuntu_x86_64-2.17.0-preview.0.tgz](../assets/opentok-ot-libotkit-ubuntu_x86_64-2.17.0-preview.0.tgz)]
for Ubuntu x86_84.

```
$ cd assets; unp opentok-ot-libotkit-ubuntu_x86_64-2.17.0-preview.0.tgz
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
$ mkdir OpenTokDemo-Basic-Video-Chat/build
```

Next step is to create the building bits using `cmake`.

```
$ cd OpenTokDemo-Basic-Video-Chat/build
$ CC=clang CXX=clang++ cmake ..
```

Note we are using `clang/clang++` compilers here.

Next step is building the code using `make`.

```
$ make
```

Once it finishes the `opentokdemo_basic_video_chat` binary is ready so just run it.

```
$ ./opentokdemo_basic_video_chat --room linuxsdk
```

End the sample application by CONTROL+C'ing in the console.
