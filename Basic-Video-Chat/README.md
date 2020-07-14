# Basic Video Chat

The Basic Video Chat application shows how to use the OpenTok Linux SDK
to connect to an OpenTok session, publish a stream, and subscribe to a stream.
It builds upon the [Publisher Only sample](../Publisher-Only) sample,
adding an OpenTok stream subscriber in addition to a publisher.

Upon building and running this sample application, you should be able to have
two-way audio and video communication using OpenTok.

You will need a valid [Vonage Video API](https://tokbox.com/developer/)
account to build this app. (Note that OpenTok is now the Vonage Video API.)

## Building and running the sample app

Edit the [main.cpp](main.cpp) file and add your OpenTok API key,
an OpenTok session ID, and token for that session.

Before building the sample application, uncompress the OpenTok Linux SDK
package in the [assets](../assets) folder. Uncompress the version for
the target host of your sample application. For example, for Ubuntu ARMv7 uncompress
the opentok-ot-libotkit-ubuntu_armv7-2.17.0-preview.0.tgz file:

```
$ cd assets; unp opentok-ot-libotkit-ubuntu_x86_64-2.17.0-preview.0.tgz
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
folder, let's go ahead and create it in the project directory:

```
$ mkdir Basic-Video-Chat/build
```

Next, create the building bits using `cmake`:

```
$ cd Basic-Video-Chat/build
$ CC=clang CXX=clang++ cmake ..
```

Note we are using `clang/clang++` compilers here.

Finally, use `make` to build the code:

```
$ make
```

Then when the `basic_video_chat` binary is built, run it:

```
$ ./basic_video_chat
```

You can use the [OpenTok Playground](https://tokbox.com/developer/tools/playground/)
to connect to the OpenTok session in a web browser, view the stream published
by the Basic Video Chat app, and publish a stream that the app can subscribe to.

To end the sample application, type Control + C in the console.

## About the code

This application builds upon the [Publisher Only sample](../Publisher-Only) sample
by subscribing to streams in the session, in addition to publishing a stream.

See the [Understanding the code section](](../Publisher-Only/README.md) of the
Publisher Only sample to see how the app connects to the OpenTok session and
publishes a stream.

### Subscribing to streams in the session

When the application creates a `otc_session_cb` structure (see
[Connecting to an OpenTok session](](../Publisher-Only/README.md#connecting-to-an-opentok-session)),
it sets the `on_stream_received()` callback function, which is defined in the
OpenTok Linux SDK:

```c
session_callbacks.on_stream_received = onOpenTokSessionStreamReceivedCallback;
```

This function is called when a stream published by another client in the session
is received. This can happen when you connect to an existing session and there are
existing streams in the session or when other clients publish new streams to a
session you have connected to.

In the implementation of this callback function (in the conference.cpp file),
the application creates a structure of type`otc_subscriber_cb`, defined in
the OpenTok Linux SDK:

```c
struct otc_subscriber_cb subscriber_cb = {0};
subscriber_cb.user_data = conference;
subscriber_cb.on_render_frame = onOpenTokSubscriberRenderFrameCallback;
subscriber_cb.on_error = onOpenTokSubscriberErrorCallback;
```

The members of the `otc_subscriber_cb` structure are each callback functions that
are called when events related to the subscribed stream occur:

* `on_render_frame` -- Called when the subscriber has a new video frame to
  render from the stream.

  * `on_error` -- Invoked when an error occurs in subscribing to the stream.

Then the code calls the `otc_subscriber_new()` function to instantiate a
`otc_subscriber` struct (defined by the OpenTok Linux SDK), representing the
subscriber:

```
otc_subscriber* subscriber = otc_subscriber_new((otc_stream*)stream, &subscriber_cb);
```

The function takes two arguments:

* `stream` -- The `otc_stream` struct for the the stream, obtained from the
  `on_stream_received` callback function.

* `callbacks` -- The `subscriber_cb` subscribers callback structure, described
  above.

This function returns an `otc_subscriber` struct and starts subscribing to the stream
(starts receiving the audio-video data from the stream).

### Rendering the subscriber video

The renderer.h and renderer.cpp files define the Renderer and RendererManager
C++ classes. The application uses these to render video frames in the user interface.

When a new video frame is available in the subscriber stream, the `on_render_frame()`
function of the `otc_subscriber_cb` structure is called. The `frame` parameter of
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

## Next steps

The [Basic Video Chat With Server application](../Basic-Video_Chat-With-Server)
sample builds upon the Basic Video Chat sample. It pulls the OpenTok session ID
and token from a web service, as you would in a real-world production application.
(In the other two samples, you use a hard-coded session ID and token, which
is not an option in a production application, since client tokens expire after
a period of time.)

See the [Vonage Video API developer center](https://tokbox.com/developer/)
for more information on the OpenTok Linux SDK.
