# Basic Video Chat

The Basic Video Chat application shows how to connect to an OpenTok session,
publish a stream, and subscribe to a stream using the OpenTok Linux SDK.

It implements a simple video call application with several clients.

Upon building and running this sample application, you should be able to have
two way audio and video communication using OpenTok.

Note that you will need a valid TokBox account for this app. Edit the
[main.cpp](main.cpp) file and add your API key, session id and token.

## Building and running the sample app

We use [CMake](https://cmake.org) in this sample application so you have to
install it before trying to build the sample application. There are other
dependencies you have to install too. These are:
  - build-essential
  - cmake
  - clang
  - libc++-dev
  - libc++abi-dev
  - pkg-config
  - libasound2
  - libpulse-dev
  - libsdl2-dev

The OpenTok Linux SDK is available as a Debian package. We maintain our own
Debian repository on Bintray. Run the following commands to install packages
from it.

* Add a new entry to your `/etc/apt/sources.list` file.
```bash
echo "deb https://dl.bintray.com/tokbox/debian buster main" | sudo tee -a /etc/apt/sources.list
```
* Add Bintray's GPG Key.
```bash
wget -O- -q https://bintray.com/user/downloadSubjectPublicKey?username=bintray | sudo apt-key add -
```
* Resynchronize the package index files from their sources.
```bash
sudo apt-get update
```
* Install the OpenTok Linux SDK packages.
```bash
sudo apt-get install libopentok-dev
```

We are all set. Let's build the sample application. Since it's good practice to
create a build folder, let's go ahead and create it in the project directory.

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
