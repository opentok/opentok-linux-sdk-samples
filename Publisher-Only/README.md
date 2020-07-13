# Publisher Only

The Publisher Only sample application is a very simple application meant to get
you started using the OpenTok Linux SDK.

Upon building and running this sample application, you should be able to publish
an audio-video stream to an OpenTok session. This sample application is
intended to be used in a tiny single-board computer such as the Raspberry Pi.

You will need a valid [Vonage Video API](https://tokbox.com/developer/)
account to build this app. (Note that OpenTok is now the Vonage Video API.)

To learn the basic concepts of using OpenTok, see
[Video API basics](https://tokbox.com/developer/guides/basics/).

## Building and running the sample app

Edit the [main.cpp](main.cpp) file and add your OpenTok API key,
an OpenTok session ID, and token for that session. Note that in a
real-world application, you would obtain the OpenTok session ID and token
from a web service, as illustrated in the
[Basic Video Chat with Server application](../Basic-Video-Chat-With-Server)
sample.

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
$ mkdir Publisher-Only/build
```

Next, create the building bits using `cmake`:

```
$ cd Publisher-Only/build
$ CC=clang CXX=clang++ cmake ..
```

Note we are using `clang/clang++` compilers here.

Finally, use `make` to build the code:

```
$ make
```

Then when the `publisher_only` binary is built, run it:

```
$ ./publisher_only
```

You can use the [OpenTok Playground](https://tokbox.com/developer/tools/playground/)
to connect to the OpenTok session in a web browser and view the stream published
by the Publisher Only app.

You can end the sample application by sending a SIGBREAK (21) signal in the console:

```
$ kill -21 <publisher_only PID>
```

## Understanding the code

The main.cpp file includes the conference.h header (in the src directory).
The conference.cpp file includes the OpenTok Linux SDK:

```c
#include "opentok.h"
```

The main.cpp file calls functions in the conference.h file, and the
implementation of these functions call OpenTok functionality defined in the
OpenTok Linus SDK (and the opentok.h file).

### Connecting to an OpenTok session

The `main()` function (in the main.cpp file) calls the `conference->join()`
function, passing in the OpenTok API key, session ID, and token.

```c
conference->join(apiKey, sessionId, token);
```

The `Conference::join()` function in turn calls the `Conference::joinImpl()`
function (in the conference.cpp file). The `joinImpl()` function first creates
a structure of type `otc_session_cb`, defined by the OpenTok Linux SDK:

```c
struct otc_session_cb session_callbacks = {0};
session_callbacks.user_data = this;
session_callbacks.on_connected = onOpenTokSessionConnectedCallback;
session_callbacks.on_stream_received = onOpenTokSessionStreamReceivedCallback;
session_callbacks.on_stream_dropped = onOpenTokSessionStreamDroppedCallback;
session_callbacks.on_error = onOpenTokSessionErrorCallback;
session_ = otc_session_new(apiKey.c_str(), sessionId.c_str(), &session_callbacks);
```

The members of the `otc_session_cb` structure are each callback functions that
are called when events related to the OpenTok session occur:

* `on_connected` -- Invoked after the `otc_session_connect()` function (see below)
  successfully connects the instance to an OpenTok session.

* `on_stream_received` -- Invoked when a there is a new stream in the OpenTok session
  (when another client publishes a stream to the session). This sample application
  does not use this callback function, since it only publishes a stream to the
  session (it does not subscribe to streams published by other clients). The other
  sample applications do use this callback.

* `on_stream_dropped` -- Invoked when another client's stream is dropped from
  the OpenTok session. This can happen when the client stops publishing the stream
  or if the client's network connection drops. This sample application
  does not use this callback function, since it only publishes a stream to the
  session (it does not subscribe to streams published by other clients). The other
  sample applications do use this callback.

  * `on_error` -- Invoked when an error occurs.

The `joinImpl()` function then calls the `otc_session_new()` function, defined by
OpenTok Linux SDK, passing in the OpenTok API key string, the OpenTok
session ID string, and a pointer to the `session_callbacks`structure:

```
session_ = otc_session_new(apiKey.c_str(), sessionId.c_str(), &session_callbacks);
```

The `otc_session_new()` function, which returns an `otc_session` structure
(defined by the OpenTok Linux SDK), which represents an OpenTok session.

The `joinImpl()` function then calls the `otc_session_connect()` function, defined by
OpenTok Linux SDK:

```c
  otc_session_connect(session_, token.c_str);
```

This function connects the client to the OpenTok session. Upon successful connection,
the `otc_session_cb.on_connected` callback function is invoked, and the application
publishes a stream to the session.

### Publishing a stream to the session

After connecting to the session, the code creates a structure of type
`otc_publisher_cb`, defined by the OpenTok Linux SDK:

```c
struct otc_publisher_cb publisher_cb = {0};
publisher_cb.user_data = this;
publisher_cb.on_stream_created = onOpenTokPublisherStreamCreatedCallback;
publisher_cb.on_render_frame = onOpenTokPublisherRenderFrameCallback;
publisher_cb.on_error = onOpenTokPublisherErrorCallback;
publisher_ = otc_publisher_new(name_.c_str(), nullptr, &publisher_cb);
```

The members of the `otc_publisher_cb` structure are each callback functions that
are called when events related to the published stream occur:

* `on_render_frame` -- Called when the publisher can render a new video frame to the stream.

* `on_stream_received` -- Invoked when a there is a new stream in the OpenTok session
  (when another client publishes a stream to the session). This sample application
  does not use this callback function, since it only publishes a stream to the
  session (it does not subscribe to streams published by other clients). The other
  sample applications do use this callback.

* `on_stream_dropped` -- Invoked when another client's stream is dropped from
  the OpenTok session. This can happen when the client stops publishing the stream
  or if the client's network connection drops. This sample application
  does not use this callback function, since it only publishes a stream to the
  session (it does not subscribe to streams published by other clients). The other
  sample applications do use this callback.

  * `on_error` -- Invoked when an error occurs in publishing the stream.

Then the code calls the `otc_publisher_new()` function to instantiate a
`otc_publisher` struct (defined by the OpenTok Linux SDK), representing the
publisher. The function takes three arguments:

* `name` -- A name (optional) identifying the publisher of the stream.

* `capturer` -- Use this parameter if you want to provide a custom video capturer.
  If it is set to `null`, as in this sample application, the publisher uses the
  defaultvideo capturer.

* `callbacks` -- The `otc_publisher_cb` publisher callback structure, described
  above.

When the application connects to the OpenTok session, the
`onOpenTokSessionConnected()` callback function is called (see the previous
section), and it calls the `otc_session_publish()` function, defined
by the OpenTok Linux SDK, to publish a stream to the OpenTok session:

```c
void Conference::onOpenTokSessionConnected() {
  sessionConnected_ = true;
  if (observer_ == nullptr) {
    return;
  }
  observer_->onConnected();
  if ((session_ != nullptr) && (publisher_ != nullptr)) {
    otc_session_publish(session_, publisher_);
  }
}
```

### Rendering the publisher video

When the published stream is ready for a new video frame, 

The renderer.h and renderer.cpp files define the Renderer and RendererManager
C++ classes. The application use these to render video frames in the UI.

When the publisher is ready for a new video frame, the `on_render_frame()`
function of the `otc_publisher_cb` struct is called. The `frame` parameter of
this function is a pointer to an `otc_video_frame` structure. When this callback
is invoked, the `RendererManager::addFrame()` function (in renderer.cpp) is
called:

```c
void RendererManager::addFrame(void* key, const otc_video_frame *frame) {
  pushEvent(kRendererDraw, key, (void*)otc_video_frame_convert(OTC_VIDEO_FRAME_FORMAT_ARGB32, frame));
}
```

The `otc_video_frame_convert()` function, defined by the OpenTok Linux SDK,
converts the supplied ARGB32 frame to an `otc_video_frame` structure (also
defined by the SDK).

The `RendererManager::handleCustomEvent()` method (defined in renderer.cpp)
handles the `kRendererDraw` event. It grabs the `otc_video_frame` structure,
and passes it into the `Renderer::onFrame` function: 

```c
otc_video_frame*  frame = (otc_video_frame*)event.user.data2;
renderers_[event.user.data1]->onFrame(frame);
otc_video_frame_delete(frame);
```

The `Renderer::onFrame` function uses the `otc_video_frame_get_plane()`
function of the OpenTok Linux SDK to convert the `otc_video_frame`
structure into an `SDL_Surface` and renders the result:

```c
void Renderer::onFrame(otc_video_frame* frame) {
  if (!window_) {
    return;
  }
  SDL_Surface* surface_ =  SDL_GetWindowSurface(window_);

  auto pixels = otc_video_frame_get_plane(frame, 0);
  SDL_Surface* sdl_frame = SDL_CreateRGBSurfaceFrom(
      const_cast<unsigned char*>(pixels),
      otc_video_frame_get_width(frame),
      otc_video_frame_get_height(frame),
      32,
      otc_video_frame_get_width(frame) * 4,
      0,0,0,0);

  SDL_BlitScaled(sdl_frame, NULL, surface_, nullptr); // blit it to the screen
  SDL_FreeSurface(sdl_frame);

  SDL_UpdateWindowSurface(window_);
}
```

### Disconnecting from the session

When the libuv event loop exits, the `main()` function calls the
`Conference::leave()` function, which does the following:

* It unpublishes the stream from the session by calling `otc_session_unpublish()`.

* It deletes the `otc_publisher` structure by calling `otc_publisher_delete()`.

* It disconnects from the OpenTok session by calling `otc_session_disconnect()`.

* It deletes the `otc_psession` structure by calling `otc_session_delete()`.

The `Conference::leave()` function also unsubscribes from any subscribed
streams. However, this Publisher Only sample does not subscribe to any streams.
See the [Basic Video Chat application](../Basic-Video_Chat) sample).

## Next steps

The [Basic Video Chat application](../Basic-Video_Chat) sample builds upon the
Publisher Only sample, adding an OpenTok stream subscriber in addition to a publisher. 

See the [Vonage Video API developer center](https://tokbox.com/developer/)
for more information on the OpenTok Linux SDK.
