# Client Observability

This application, built on top of Basic Video Chat, shows how to [retrieve statistics](https://tokbox.com/developer/guides/client-observability/) from publishers and subscribers.

The OpenTok Linux SDK provides calls to access real-time network and media statistics in a video session. These calls report detailed stream quality metrics—such as packet loss, data received, and bandwidth—and can be used on any publisher or subscribed stream. </p>

In addition to [publisher](https://tokbox.com/developer/guides/publish-stream/linux/#get_stats) and [subscriber statistics](https://tokbox.com/developer/guides/subscribe-stream/linux/#stream_info)—which cover common metrics as well as detailed RTC stats reports—the OpenTok SDK exposes sender-side statistics, allowing receivers to monitor the sender’s connection performance in real time.

## Setting up your environment

### OpenTok SDK

Building this sample application requires having a local installation of the
OpenTok Linux SDK.

#### On Debian-based Linuxes

The OpenTok Linux SDK for x86_64 is available as a Debian
package. For Debian we support Debian 12 (Bookworm). We maintain
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
wget https://tokbox.com/downloads/libopentok_linux_llvm_x86_64-2.31.1
tar xvf libopentok_linux_llvm_x86_64-2.31.1
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
$ mkdir Basic-Video-Chat/build
```

Copy the [config-sample.h](../common/src/config-sample.h) file as `config.h` at
`Basic-Video-Chat/`:

```bash
$ cp common/src/config-sample.h  Basic-Video-Chat/config.h
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
$ cd Basic-Video-Chat/build
$ CC=clang CXX=clang++ cmake ..
```

Note we are using `clang/clang++` compilers.

Use `make` to build the code:

```bash
$ make
```

When the `basic_video_chat` binary is built, run it:

```bash
$ ./basic_video_chat
```

You can use the [OpenTok Playground](https://tokbox.com/developer/tools/playground/)
to connect to the OpenTok session in a web browser, view the stream published
by the Basic Video Chat app, and publish a stream that the app can subscribe to.

You can end the sample application by typing Control + C in the console.

## Understanding the code

### Receiving statistics events

To receive statistics events, you need to set the appropriate callbacks. For example, to receive audio and video statistics for a publisher, you can configure the callbacks like this:

```c
otc_publisher_callbacks publisher_callbacks = {0};
publisher_callbacks.on_audio_stats = on_publisher_audio_stats;
publisher_callbacks.on_video_stats = on_publisher_video_stats;
```

And for a subscriber:
```c
otc_subscriber_callbacks subscriber_callbacks = {0};
subscriber_callbacks.on_video_stats = on_subscriber_video_stats;
subscriber_callbacks.on_audio_stats = on_subscriber_audio_stats;
```

Then, access each field within the corresponding callback. For example:
```c
static void on_subscriber_video_stats(otc_subscriber *subscriber,
                                      void *user_data,
                                      struct otc_subscriber_video_stats video_stats)
{
    std::cout << __FUNCTION__ << " callback function" << std::endl;
    std::cout << "Subscriber video bytes received: " << video_stats.bytes_received << std::endl;
    std::cout << "Subscriber video packets received: " << video_stats.packets_received << std::endl;
    std::cout << "Subscriber video packets lost: " << video_stats.packets_lost << std::endl;
    // Check if sender statistics are available and print
    if (video_stats.sender_connection_estimated_bandwidth >= 0
        && video_stats.sender_connection_max_allocated_bitrate >= 0) {
        std::cout << "Sender connection estimated bandwidth: "
                  << video_stats.sender_connection_estimated_bandwidth << std::endl;
        std::cout << "Sender connection max allocated bitrate: "
                  << video_stats.sender_connection_max_allocated_bitrate << std::endl;
    }
}
```

### Enabling sender-side statistics

To use sender-side statistics, enable them for the stream’s publisher by calling `otc_publisher_settings_set_sender_stats_track()` function before constructing the publisher:

```c
otc_publisher_settings* settings = otc_publisher_settings_new();
otc_publisher_settings_set_sender_stats_track(settings, OTC_TRUE);

otc_publisher* publisher = otc_publisher_new_with_settings( &publisher_callbacks, settings);
```

