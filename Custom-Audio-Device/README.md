# Custom Audio Device

The Custom Audio Device sample application shows how to use a custom audio
device in your application. A custom audio device lets you implement your own
way of either capturing or rendering audio or both. It is important to note that
only a custom audio device can be set at a time in an application.

This sample builds upon the [Publisher Only](../Publisher-Only) sample, using
a custom audio device that implements a custom way of capturing generated audio
samples.

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
$ mkdir Custom-Audio-Device/build
```

Copy the [config-sample.h](../common/src/config-sample.h) file as `config.h` at
`Custom-Audio-Device/`:

```
$ cp common/src/config-sample.h  Custom-Audio-Device/
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
$ cd Custom-Audio-Device/build
$ CC=clang CXX=clang++ cmake ..
```

Note we are using `clang/clang++` compilers.

Use `make` to build the code:

```
$ make
```

When the `custom_audio_device` binary is built, run it:

```
$ ./custom_audio_device
```

You can use the [OpenTok Playground](https://tokbox.com/developer/tools/playground/)
to connect to the OpenTok session in a web browser, view the stream published
by the Custom Audio Device app, and publish a stream that the app can subscribe to.

You can end the sample application by typing Control + C in the console.

## Understanding the code

The main.cpp file includes the OpenTok Linux SDK header:

```c
#include "opentok.h"
```

We will focus here in the custom audio device implementation. This sample builds
upon the [Publisher Only](../Publisher-Only) sample.

### Implementing the custom audio device

A custom audio device is represented by an `otc_audio_device` struct. And an
`otc_audio_device_callbacks` struct includes function pointers to functions
that act as the audio-related callbacks that the OpenTok Linux SDK invokes.

The implementation of the custom audio device in this sample defines a custom
audio capturer. It does not set up a custom audio renderer, but its implementation
would be similar.

This sample uses a `struct audio_device` user-defined struct that
defines everything needed by the custom audio capturer:

```c
struct audio_device {
  otc_audio_device_callbacks audio_device_callbacks;
  otk_thread_t capturer_thread;
  std::atomic<bool> capturer_thread_exit;
};
```

The application initializes it and sets the pointers to callback functions:

```
struct audio_device *device = (struct audio_device *)malloc(sizeof(struct audio_device));
device->audio_device_callbacks = {0};
device->audio_device_callbacks.user_data = static_cast<void *>(device);
device->audio_device_callbacks.destroy_capturer = audio_device_destroy_capturer;
device->audio_device_callbacks.start_capturer = audio_device_start_capturer;
device->audio_device_callbacks.get_capture_settings = audio_device_get_capture_settings;
```

The `audio_device_start_capturer` function starts a thread
in which audio is captured:

```
static otc_bool audio_device_start_capturer(const otc_audio_device *audio_device,
                                            void *user_data) {
  struct audio_device *device = static_cast<struct audio_device *>(user_data);
  if (device == nullptr) {
    return OTC_FALSE;
  }

  device->capturer_thread_exit = false;
  if (otk_thread_create(&(device->capturer_thread), &capturer_thread_start_function, (void *)device) != 0) {
    return OTC_FALSE;
  }

  return OTC_TRUE;
}
```

The `capturer_thread_start()` function creates an array of 480 16-bit audio samples
(from a generated waveform) and passes the array to the
`otc_audio_device_write_capture_data()` function, defined in the OpenTok Linux
SDK:

``c
static otk_thread_func_return_type capturer_thread_start_function(void *arg) {
  struct audio_device *device = static_cast<struct audio_device *>(arg);
  if (device == nullptr) {
    otk_thread_func_return_value;
  }

  int16_t samples[480];
  static double time = 0;

  while (device->capturer_thread_exit.load() == false) {
    for (int i = 0; i < 480; i++) {
      double val = (INT16_MAX  * 0.75) * cos(2.0 * M_PI * 4.0 * time / 10.0 );
      samples[i] = (int16_t)val;
      time += 10.0 / 480.0;
    }
    otc_audio_device_write_capture_data(samples, 480);
    usleep(10 * 1000);
  }

  otk_thread_func_return_value;
}
```

The `otc_audio_device_write_capture_data()` function adds the audio byte
array to the audio buffer that will be used for audio in the published stream.

### Setting the audio device to be used by the application

Once the application initializes the `struct audio_device` struct and sets the
pointers to the callback functions, the custom audio device can be set by
using the `otc_set_audio_device()` function defined in the OpenTok Linux SDK.

```c
  otc_set_audio_device(&(device->audio_device_callbacks));
```

The function takes a pointer to the `audio_device_callbacks` struct
(of type `otc_audio_device_callbacks`) defined in the previous section.

### Configuring the audio device

The OpenToken Linux SDK calls the `get_capture_settings` callback function
that we added to the `audio_device_callbacks` struct. In this callback, 
we adjust configuration settings for the audio capturer:
 
```
static otc_bool audio_device_get_capture_settings(const otc_audio_device *audio_device,
                                                  void *user_data,
                                                  struct otc_audio_device_settings *settings) {
  if (settings == nullptr) {
    return OTC_FALSE;
  }

  settings->number_of_channels = 1;
  settings->sampling_rate = 48000;
  return OTC_TRUE;
}
```

The app sets the sampling rate to 48000 (samples per second). Note that the
custom audio device implementation (described above) sets 480 audio samples
each 10 microseconds (48,000 per second).

### 

## Next steps

See the [Vonage Video API developer center](https://tokbox.com/developer/)
for more information on the OpenTok Linux SDK.

See the [Adjusting audio and video -- Linux](https://tokbox.com/developer/guides/audio-video/linux)
developer for more information on setting custom audio captures and renderers,
as well as other audio options, using the OpenTok Linux SDK.
