# Basic Video Chat

The Basic Video Chat application shows how to use the OpenTok Linux SDK
to connect to an OpenTok session, publish a stream, and subscribe to a stream.
It builds upon the [Publisher Only sample](../Publisher-Only) sample,
adding an OpenTok stream subscriber in addition to a publisher.

Upon building and running this sample application, you should be able to have
two-way audio and video communication using OpenTok.

You will need a valid [Vonage Video API](https://tokbox.com/developer/)
account to build this app. (Note that OpenTok is now the Vonage Video API.)

## Building and running the sample app

Edit the [main.cpp](main.cpp) file and add your OpenTok API key,
an OpenTok session ID, and token for that session.

Before building the sample application, uncompress the OpenTok Linux SDK
package in the [assets](../assets) folder. Uncompress the version for
the target host of your sample application. For example, for Ubuntu ARMv7 uncompress
the opentok-ot-libotkit-ubuntu_armv7-2.17.0-preview.0.tgz file:

```
$ cd assets; unp opentok-ot-libotkit-ubuntu_x86_64-2.17.0-preview.0.tgz
```

This creates a `package` folder at [assets/package](../assets/package). The
sample application build bits will try find the OpenTok Linux SDK header files
and library under the `package` folder.

This application uses [CMake](https://cmake.org). Before building the sample application,
install it and these other dependencies:

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
folder, let's go ahead and create it in the project directory:

```
$ mkdir Basic-Video-Chat/build
```

Next, create the building bits using `cmake`:

```
$ cd Basic-Video-Chat/build
$ CC=clang CXX=clang++ cmake ..
```

Note we are using `clang/clang++` compilers here.

Finally, use `make` to build the code:

```
$ make
```

Then when the `basic_video_chat` binary is built, run it:

```
$ ./basic_video_chat
```

You can use the [OpenTok Playground](https://tokbox.com/developer/tools/playground/)
to connect to the OpenTok session in a web browser, view the stream published
by the Basic Video Chat app, and publish a stream that the app can subscribe to.

To end the sample application, type Control + C in the console.

## About the code

To do.

## Next steps

The [Basic Video Chat application](../Basic-Video_Chat) sample builds upon the
Publisher Only sample, adding an OpenTok stream subscriber in addition to a publisher. 

See the [Vonage Video API developer center](https://tokbox.com/developer/)
for more information on the OpenTok Linux SDK.
