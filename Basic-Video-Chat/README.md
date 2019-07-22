# Basic Video Chat

The Basic Video Chat application shows how to connect to an OpenTok session,
publish a stream, and subscribe to a stream using the OpenTok Linux SDK.

Upon building and running this sample application, you should be able to have
two way audio and video communication using OpenTok.

Note that you will need a valid TokBox account for this app. Edit the
[main.cpp](main.cpp) file and add your API key, session id and token.

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

Let's build the sample application. Since it's good practice to create a build
folder, let's go ahead and create it in the project directory.

```
$ mkdir Basic-Video-Chat/build
```

Next step is to create the building bits using `cmake`.

```
$ cd Basic-Video-Chat/build
$ CC=clang CXX=clang++ cmake ..
```

Note we are using `clang/clang++` compilers here.

We will use `make` to build the code.

```
$ make
```

Run the `basic_video_chat` binary once it's built.

```
$ ./basic_video_chat
```

You can end the sample application by using the Control + C combination in the console.
