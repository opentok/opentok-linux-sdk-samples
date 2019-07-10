# Basic Video Chat With Server

The Basic Video Chat app is a very simple application meant to get a new
developer started using the OpenTok Linux SDK.

Upon building and running this sample application, you should be able to have
two way audio and video communication using OpenTok.

The new thing here is that you are using a server to create sessions, generate
tokens for those sessions, archive (or record) sessions, and download those
archives.

## Setting up the test web service

The [Learning OpenTok PHP](https://github.com/opentok/learning-opentok-php) repo
includes code for setting up a web service.

1. Clone or download the repo and run its code on a PHP-enabled web server. If you do not have a
   PHP server set up, you can use Heroku to run a remote test server -- see [Automatic deployment
   to Heroku](https://github.com/opentok/learning-opentok-php#automatic-deployment-to-heroku).

2. After getting this web service running, edit the main.cpp file and set the value for
   `SERVER_URL` to the URL for the web service:

   * If you deployed a the test web service to a local PHP server, set this to the following:

```c
#define SERVER_URL "http://localhost:8080"
```
   * If you deployed this to Heroku, set this to the following:

```c
#define SERVER_URL "YOUR-HEROKU-APP-URL"
```
   ***Do not add the trailing slash of the URL.***

The sample will load the OpenTok session ID, token, and API key from the web service. Also,
the archiving sample will use the web service to start, stop, and view archives.

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
$ mkdir Basic-Video-Chat-With-Server/build
```

Next step is to create the building bits using `cmake`.

```
$ cd Basic-Video-Chat-With-Server/build
$ CC=clang CXX=clang++ cmake ..
```

Note we are using `clang/clang++` compilers here.

Next step is building the code using `make`.

```
$ make
```

Once it finishes the `basic_video_chat_with_server` binary is ready so just run it.

```
$ ./basic_video_chat_with_server --room linuxsdk
```

End the sample application by CONTROL+C'ing in the console.
