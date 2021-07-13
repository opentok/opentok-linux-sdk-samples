# Custom Video Capturer

The Custom Video Capture sample application shows how to use a custom video
capturer being used by a publisher in your application. It is important to note
that a custom video capturer always belongs to a given publisher.

This sample builds upon the [Publisher Only](../Publisher-Only) sample, but with
a publisher that uses a custom video capturer. The custom video capturer in this
samples creates generated video frames that the publisher will send to other
participants in the session.

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
$ mkdir Custom-Video-Capturer/build
```

Copy the [config-sample.h](../common/src/config-sample.h) file as `config.h` at
`Custom-Video-Capturer/`:

```bash
$ cp common/src/config-sample.h  Custom-Video-Capturer/config.h
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
$ cd Custom-Video-Capturer/build
$ CC=clang CXX=clang++ cmake ..
```

Note we are using `clang/clang++` compilers.

Use `make` to build the code:

```bash
$ make
```

When the `custom_video_capturer` binary is built, run it:

```bash
$ ./custom_video_capturer
```

You can use the [OpenTok Playground](https://tokbox.com/developer/tools/playground/)
to connect to the OpenTok session in a web browser, view the stream published
by the Custom Video Capturer app, and publish a stream that the app can subscribe to.

You can end the sample application by typing Control + C in the console.

## Understanding the code

The main.cpp file includes the OpenTok Linux SDK header:

```c
#include "opentok.h"
```

We will focus here in the custom video capture implementation and how to set it
for a given publisher. This sample builds upon the
[Publisher Only](../Publisher-Only) sample.

### Implementing the custom video capturer

A custom video capturer is represented by an  `otc_video_capturer` struct. And an
an `otc_video_capturer_callbacks` struct includes function pointers to callback
act as the video-related callbacks that the OpenTok Linux SDK invokes.

This sample uses a `struct custom_video_capturer` user-defined struct that
defines everything needed by the custom video capturer:

```c
struct custom_video_capturer {
  const otc_video_capturer *video_capturer;
  struct otc_video_capturer_callbacks video_capturer_callbacks;
  int width;
  int height;
  otk_thread_t capturer_thread;
  std::atomic<bool> capturer_thread_exit;
};
```

The application initializes it and sets the pointers to callback functions:

```c
struct custom_video_capturer *video_capturer = (struct custom_video_capturer *)malloc(sizeof(struct custom_video_capturer));
video_capturer->video_capturer_callbacks = {0};
video_capturer->video_capturer_callbacks.user_data = static_cast<void *>(video_capturer);
video_capturer->video_capturer_callbacks.init = video_capturer_init;
video_capturer->video_capturer_callbacks.destroy = video_capturer_destroy;
video_capturer->video_capturer_callbacks.start = video_capturer_start;
video_capturer->video_capturer_callbacks.get_capture_settings = get_video_capturer_capture_settings;
video_capturer->width = 1280;
video_capturer->height = 720;
```

The `video_capturer_start` function starts a thread in which video frames are captured:

```c
static otc_bool video_capturer_start(const otc_video_capturer *capturer, void *user_data) {
  struct custom_video_capturer *video_capturer = static_cast<struct custom_video_capturer *>(user_data);
  if (video_capturer == nullptr) {
    return OTC_FALSE;
  }

  video_capturer->capturer_thread_exit = false;
  if (otk_thread_create(&(video_capturer->capturer_thread), &capturer_thread_start_function, (void *)video_capturer) != 0) {
    return OTC_FALSE;
  }

  return OTC_TRUE;
}
```

The thread start function creates buffer of data for generated video frames.
The code calls the `otc_video_frame_new()` function, defined by the OpenTok Linux
SDK, to write an ARGB32 image to the a video frame. Video frames are provided
to the custom video capturer by calling the `otc_video_capturer_provide_frame()`
function, defined in the OpenTok Linux SDK:

```c
static otk_thread_func_return_type capturer_thread_start_function(void *arg) {
  struct custom_video_capturer *video_capturer = static_cast<struct custom_video_capturer *>(arg);
  if (video_capturer == nullptr) {
    otk_thread_func_return_value;
  }

  uint8_t *buffer = (uint8_t *)malloc(sizeof(uint8_t) * video_capturer->width * video_capturer->height * 4);

  while(video_capturer->capturer_thread_exit.load() == false) {
    memset(buffer, generate_random_integer() & 0xFF, video_capturer->width * video_capturer->height * 4);
    otc_video_frame *otc_frame = otc_video_frame_new(OTC_VIDEO_FRAME_FORMAT_ARGB32, video_capturer->width, video_capturer->height, buffer);
    otc_video_capturer_provide_frame(video_capturer->video_capturer, 0, otc_frame);
    if (otc_frame != nullptr) {
      otc_video_frame_delete(otc_frame);
    }
    usleep(1000 / 30 * 1000);
  }

  if (buffer != nullptr) {
    free(buffer);
  }

  otk_thread_func_return_value;
}
```

### Setting the video capturer to be used by the publisher

When the publisher is created, the custom video capturer has been set.
A pointer to the `otc_video_capturer_callbacks` struct is passed into
the `otc_publisher_new()` function, which creates the publisher: 

```c
g_publisher = otc_publisher_new("opentok-linux-sdk-samples",
								&(video_capturer->video_capturer_callbacks),
								&publisher_callbacks);
```

The `otc_publisher_new()` function is defined in the OpenTok Linux SDK.

## Next steps

See the [Vonage Video API developer center](https://tokbox.com/developer/)
for more information on the OpenTok Linux SDK.
