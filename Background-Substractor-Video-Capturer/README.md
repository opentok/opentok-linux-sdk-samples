# Background Substractor Video Capturer

This sample application shows how to use a video capturer being used by a
publisher in your application. It is important to note that a custom video
capturer always belongs to a given publisher. The video capturer captures
video frames from the default video camera, removes its background and provides
them to the SDK so they are sent to participants in the session.

This sample builds upon the [Publisher Only](../Publisher-Only) sample, but with
a publisher that uses a backgroung substractor stream video capturer where
background subtraction is performed through OpenCV.

You will need a valid [Vonage Video API](https://tokbox.com/developer/)
account to build this app. (Note that OpenTok is now the Vonage Video API.)

## Setting up your environment

### OpenTok SDK

Building this sample application requires having a local installation of the
OpenTok Linux SDK.

#### On Debian-based Linuxes

The OpenTok Linux SDK for x86_64 is available as a Debian
package. For Debian we support Debian 11 (Bullseye). We maintain
our own Debian repository on packagecloud. Follow these steps
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
wget https://tokbox.com/downloads/libopentok_linux_llvm_x86_64-2.30.1
tar xvf libopentok_linux_llvm_x86_64-2.30.1
export LIBOPENTOK_PATH=<path_to_SDK>
```

## Other dependencies

Before building the sample application you will need to install the following dependencies

### On Debian-based Linuxes

```bash
sudo apt install build-essential cmake clang libc++-dev libc++abi-dev \
    pkg-config libasound2 libpulse-dev libsdl2-dev libopencv-dev
```

### On Fedora

```bash
sudo dnf groupinstall "Development Tools" "Development Libraries"
sudo dnf install SDL2-devel clang pkg-config libcxx-devel libcxxabi-devel cmake opencv-devel
```

## Building and running the sample app


Once you have installed the dependencies, you can build the sample application.
Since it's good practice to create a build folder, let's go ahead and create it
in the project directory:

```bash
mkdir Background-Substractor-Video-Capturer/build
```

Copy the [config-sample.h](../common/src/config-sample.h) file as `config.h` at
`Background-Substractor-Video-Capturer/`:

```bash
cp common/src/config-sample.h  Background-Substractor-Video-Capturer/config.h
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
cd Background-Substractor-Video-Capturer/build
CC=clang CXX=clang++ cmake ..
```

Note we are using `clang/clang++` compilers.

Use `make` to build the code:

```bash
make
```

When the `background_substractor_video_capturer` binary is built, run it:

```bash
./background_substractor_video_capturer
```

You can use the [OpenTok Playground](https://tokbox.com/developer/tools/playground/)
to connect to the OpenTok session in a web browser, view the stream published
by the Background Substractor Video Capturer app, and publish a stream that the
app can subscribe to.

You can end the sample application by typing Control + C in the console.

## Next steps

See the [Vonage Video API developer center](https://tokbox.com/developer/)
for more information on the OpenTok Linux SDK.
