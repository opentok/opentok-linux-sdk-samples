# Signaling

The Signaling sample application shows how to use the OpenTok
[signaling](https://tokbox.com/developer/guides/signaling/) feature.
This lets clients connected to an OpenTok session send data to other
clients connected to the session.

You will need a valid [Vonage Video API](https://tokbox.com/developer/)
account to build this app. (Note that OpenTok is now the Vonage Video API.)

## Building and running the sample app

This application uses [CMake](https://cmake.org). Before building
the sample application, install it and these other dependencies:

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

Once you have installed the dependencies, you can build the sample application.
Since it's good practice to create a build folder, let's go ahead and create it
in the project directory:

```
$ mkdir Signaling/build
```

Copy the [config-sample.h](../common/src/config-sample.h) file as `config.h` at
`Signaling/`:

```
$ cp common/src/config-sample.h  Signaling/
```

Edit the `config.h` file and add your OpenTok API key,
an OpenTok session ID, and token for that session. For test purposes,
you can obtain a session ID and token from the project page in your
[Vonage Video API](https://tokbox.com/developer/) account. However,
in a production application, you will need to dynamically obtain the session
ID and token from a web service that uses one of
the [Vonage Video API server SDKs](https://tokbox.com/developer/sdks/server/).

Next, create the building bits using `cmake`:

```
$ cd Signaling/build
$ CC=clang CXX=clang++ cmake ..
```

Note we are using `clang/clang++` compilers.

Use `make` to build the code:

```
$ make
```

When the `signaling` binary is built, run it:

```
$ ./signaling
```

You can use the [OpenTok Playground](https://tokbox.com/developer/tools/playground/)
to connect to the OpenTok session in a web browser, view the stream published
by the Basic Video Chat app, and publish a stream that the app can subscribe to.

You can end the sample application by typing Control + C in the console.

## Understanding the code

The main.cpp file includes the OpenTok Linux SDK header:

```c
#include "opentok.h"
```

### Connecting to the session

See the [Understanding the code section](../Basic-Video-Chat/README.md#understanding-the-code)
of the Basic Video Chat application's README file how the code uses the OpenTok Linux SDK
to connect to an OpenTok session.

### Adding signal-related callbacks

When the application initializes the `otc_session_callbacks` structure, it
adds a callback function to the `on_signal_received` method:

```c
struct otc_session_callbacks session_callbacks = {0};
// ...
// This sample listens for signals.
session_callbacks.on_signal_received = on_session_signal_received;
// ...
```

### Sending and receiving signals in the session

The implementation of the `session_callbacks.on_signal_received` callback
function is invoked when the application receives an incoming signal from
another client in the OpenTok session:

```c
static void on_session_signal_received(otc_session *session,
                                       void *user_data,
                                       const char *type,
                                       const char *signal,
                                       const otc_connection *connection) {
  std::cout << __FUNCTION__ << " callback function" << std::endl;

  if (session == nullptr) {
    return;
  }
  // It echoes back whatever is sent to it.
  otc_session_send_signal_to_connection(session, type, signal, connection);
}
```

It then calls the `otc_session_send_signal_to_connection()` function,
defined in the OpenTok Linux SDK, to send a signal back to the client that
sent the signal. (It echos the same information back to the client.)

## Next steps

The [Basic Video Chat application](../Basic-Video_Chat) sample builds upon the
Publisher Only sample, adding an OpenTok stream subscriber in addition to a publisher. 

See the [Vonage Video API developer center](https://tokbox.com/developer/)
for more information on the OpenTok Linux SDK.
