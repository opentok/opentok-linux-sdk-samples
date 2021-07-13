# Signaling

The Signaling sample application shows how to use the OpenTok
[signaling](https://tokbox.com/developer/guides/signaling/) feature.
This lets clients connected to an OpenTok session send data to other
clients connected to the session.

You will need a valid [Vonage Video API](https://tokbox.com/developer/)
account to build this app. (Note that OpenTok is now the Vonage Video API.)

## Setting up your environment

### OpenTok SDK

Building this sample application requires having a local installation of the
OpenTok Linux SDK.

#### On Debian-based Linuxes

The OpenTok Linux SDK for x86_64 is available as a Debian
package. For Debian we support Debian 9 (Strech) and 10 (Buster). We maintain
our own Debian repository on packagecloud. For Debian 10, follow these steps
to install the packages from our repository.

* Add packagecloud repository:

```bash
curl -s https://packagecloud.io/install/repositories/tokbox/debian/script.deb.sh | sudo bash
```

* Install the OpenTok Linux SDK packages.

```bash
sudo apt install libopentok-dev
```

#### On non-Debian-based Linuxes

Download the OpenTok SDK from [https://tokbox.com/developer/sdks/linux/](https://tokbox.com/developer/sdks/linux/)
and extract it and set the `LIBOPENTOK_PATH` environment variable to point to the path where you extracted the SDK.
For example:

```bash
wget https://tokbox.com/downloads/libopentok_linux_llvm_x86_64-2.19.1
tar xvf libopentok_linux_llvm_x86_64-2.19.1
export LIBOPENTOK_PATH=<path_to_SDK>
```

## Other dependencies

Before building the sample application you will need to install the following dependencies

### On Debian-based Linuxes

```bash
sudo apt install build-essential cmake clang libc++-dev libc++abi-dev \
    pkg-config libasound2 libpulse-dev libsdl2-dev
```

### On Fedora

```bash
sudo dnf groupinstall "Development Tools" "Development Libraries"
sudo dnf install SDL2-devel clang pkg-config libcxx-devel libcxxabi-devel cmake
```

## Building and running the sample app

Once you have installed the dependencies, you can build the sample application.
Since it's good practice to create a build folder, let's go ahead and create it
in the project directory:

```bash
$ mkdir Signaling/build
```

Copy the [config-sample.h](../common/src/config-sample.h) file as `config.h` at
`Signaling/`:

```bash
$ cp common/src/config-sample.h  Signaling/config.h
```

Edit the `config.h` file and add your OpenTok API key,
an OpenTok session ID, and token for that session. For test purposes,
you can obtain a session ID and token from the project page in your
[Vonage Video API](https://tokbox.com/developer/) account. However,
in a production application, you will need to dynamically obtain the session
ID and token from a web service that uses one of
the [Vonage Video API server SDKs](https://tokbox.com/developer/sdks/server/).

Next, create the building bits using `cmake`:

```bash
$ cd Signaling/build
$ CC=clang CXX=clang++ cmake ..
```

Note we are using `clang/clang++` compilers.

Use `make` to build the code:

```bash
$ make
```

When the `signaling` binary is built, run it:

```bash
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
