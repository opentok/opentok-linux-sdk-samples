# Publisher Only

The Publisher Only sample application is a very simple application meant to get
a new developer started using the OpenTok Linux SDK.

Upon building and running this sample application, you should be able to have
one way audio and video communication using OpenTok. This sample application is
intended to be used in a tiny single-board computer such as the Raspberry Pi.

Note that you will need a valid TokBox account for this app. Edit the
[main.cpp](main.cpp) file and add your API key, session id and token.

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

Let's build the sample application. Since it's good practice to create a build
folder, let's go ahead and create it in the project directory.

```
$ mkdir Publisher-Only/build
```

Next step is to create the building bits using `cmake`.

```
$ cd Publisher-Only/build
$ CC=clang CXX=clang++ cmake ..
```

Note we are using `clang/clang++` compilers here.

We will use `make` to build the code.

```
$ make
```

Run the `publisher_only` binary once it's built.

```
$ ./publisher_only
```

You can end the sample application by sending a SIGBREAK (21) signal in the console.

```
$ kill -21 <publisher_only PID>
```
