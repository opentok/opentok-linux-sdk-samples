# Basic Video Chat With Server

The Basic Video Chat With Server application shows how to connect to an OpenTok session,
publish a stream, and subscribe to a stream using the OpenTok Linux SDK.
It builds upon the [Basic Video Chat sample](../Basic-Video-Chat) sample
by obtaining the OpenTok session ID and token from a server.

## Setting up the test web service

The [Learning OpenTok PHP](https://github.com/opentok/learning-opentok-php) repo
includes code for setting up a web service.

1. Clone or download the repo and run its code on a PHP-enabled web server. If you do not have a
   PHP server set up, you can use Heroku to run a remote test server -- see [Automatic deployment
   to Heroku](https://github.com/opentok/learning-opentok-php#automatic-deployment-to-heroku).

2. After getting this web service running, edit this sample application's main.cpp file,
   and set the value for `SERVER_URL` to the URL for the web service:

   * If you deployed a the test web service to a local PHP server, set this to the following:

      ```c
      #define SERVER_URL "http://localhost:8080"
      ```

   * If you deployed this to Heroku, set this to the following:

     ```c
     #define SERVER_URL "YOUR-HEROKU-APP-URL"
     ```

   ***Important:*** Do not add the trailing slash of the URL.

The sample will load the OpenTok session ID, token, and API key from the web service.

## Building and running the sample app

Before building the sample application, uncompress the OpenTok Linux SDK
package in the [assets](../assets) folder. Uncompress the version for
the target host of your sample application. For example, for Ubuntu ARMv7 uncompress
the opentok-ot-libotkit-ubuntu_armv7-2.17.0-preview.0.tgz file:

```
$ cd assets; unp opentok-ot-libotkit-ubuntu_armv7-2.17.0-preview.0.tgz
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
folder, go ahead and create it in the project directory:

```
$ mkdir Basic-Video-Chat-With-Server/build
```

Next, create the building bits using `cmake`:

```
$ cd Basic-Video-Chat-With-Server/build
$ CC=clang CXX=clang++ cmake ..
```

Note we are using `clang/clang++` compilers here.

Finally, use `make` to build the code:

```
$ make
```

Then when the `basic_video_chat_with_server` binary is built, run it:

```
$ ./basic_video_chat_with_server --room linuxsdk
```
You can use the [OpenTok Playground](https://tokbox.com/developer/tools/playground/)
to connect to the OpenTok session in a web browser and view the stream published
by the Publisher Only app.

To end the sample application, type Control + C in the console.

## About the code

See the [Publisher Only](../Publisher-Only) amd [Basic Video Chat](Basic-Video-Chat)
samples (and their README files) to learn how to publish and subscribe to streams
in a session.

The Basic Video Chat With Server application builds upon those samples by
obtaining the OpenTok session ID and token from a server.

The `main()` function (in the main.cpp file) calls the
`Conference::join(room, credentialServiceProviderURL)` function (defined in the
conference.h file):

```c
conference->join(roomInfo.name_, std::string(SERVER_URL));
```

This function takes two parameters: a room name (string), which you pass in as
a command line argument to the application, and a server URL (string) for the
web service that provides the OpenTok session ID (based on the room) and token
for the application to use to connect to the OpenTok session.

The implementation of the `Conference::join()` function uses the
`CredentialServiceProviderHelper` class (defined in the conference_service_provider_helper.h
file) to load this data from the web service, using curl (in the implementation).

## Next steps

See the [Vonage Video API developer center](https://tokbox.com/developer/)
for more information on the OpenTok Linux SDK.
