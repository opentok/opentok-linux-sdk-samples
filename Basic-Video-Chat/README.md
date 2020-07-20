# Basic Video Chat

The Basic Video Chat application shows how to connect to an OpenTok session,
publish a stream, and subscribe to a stream using the OpenTok Linux SDK.

It implements a simple video call application with several clients.

Upon building and running this sample application, you should be able to have
two-way audio-video communication using OpenTok.

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
$ mkdir Basic-Video-Chat/build
```

Copy the [config-sample.h](../common/src/config-sample.h) file as `config.h` at
`Basic-Video-Chat/`:

```
$ cp common/src/config-sample.h  Basic-Video-Chat/
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
$ cd Basic-Video-Chat/build
$ CC=clang CXX=clang++ cmake ..
```

Note we are using `clang/clang++` compilers.

Use `make` to build the code:

```
$ make
```

When the `basic_video_chat` binary is built, run it:

```
$ ./basic_video_chat
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

### Instantiating an otc_session instance and session-related callbacks

The `main()` function first creates a structure of type `otc_session_callbacks`,
defined in the OpenTok Linux SDK:

```c
struct otc_session_callbacks session_callbacks = {0};
session_callbacks.user_data = &renderer_manager;
session_callbacks.on_connected = on_session_connected;
session_callbacks.on_connection_created = on_session_connection_created;
session_callbacks.on_connection_dropped = on_session_connection_dropped;
session_callbacks.on_stream_received = on_session_stream_received;
session_callbacks.on_stream_dropped = on_session_stream_dropped;
session_callbacks.on_disconnected = on_session_disconnected;
session_callbacks.on_error = on_session_error;
```

Use the `user_data` member of the `otc_session_callbacks` structure to
set data associated with the instance. In this application, we set it to
a pointer to a `renderer_manager` object (described later).

The other members of the `otc_session_callbacks` structure are callback functions that
are called when events related to the OpenTok session occur:

* `on_connected` -- Invoked after the `otc_session_connect()` function (see below)
  successfully connects the instance to an OpenTok session.

* `on_connection_created` -- Invoked when another client connects to the
  OpenTok session. It is also invoked when you connect to the OpenTok session
  to report the existing clients connected to the session.

* `on_connection_dropped` -- Invoked when another client disconnects from
   the OpenTok session.

* `on_stream_received` -- Invoked when a there is a new stream in the OpenTok session
  (when another client publishes a stream to the session).

* `on_stream_dropped` -- Invoked when another client's stream is dropped from
  the OpenTok session. This can happen when the client stops publishing the stream
  or if the client's network connection drops.

  * `on_disconnected` -- Invoked when a the application disconnects from the
    OpenTok session.

  * `on_error` -- Invoked when an error occurs in connecting to the session.

The application then calls the `otc_session_new()` function, defined in
OpenTok Linux SDK, passing in the OpenTok API key string, the OpenTok
session ID string, and a pointer to the `session_callbacks`structure:

```
otc_session *session = nullptr;
session = otc_session_new(API_KEY, SESSION_ID, &session_callbacks);
```

The `otc_session_new()` function, which returns an `otc_session` structure
(defined in the OpenTok Linux SDK), which represents an OpenTok session.


### Instantiating an audio-video publisher and publisher-related callbacks

After connecting to the session, the code creates a structure of type
`otc_publisher_callbacks`, defined in the OpenTok Linux SDK:

```c
struct otc_publisher_callbacks publisher_callbacks = {0};
publisher_callbacks.user_data = &renderer_manager;
publisher_callbacks.on_stream_created = on_publisher_stream_created;
publisher_callbacks.on_render_frame = on_publisher_render_frame;
publisher_callbacks.on_stream_destroyed = on_publisher_stream_destroyed;
publisher_callbacks.on_error = on_publisher_error;
```

Use the `user_data` member of the `otc_publisher_callbacks` structure to
set data associated with the instance. In this application, we set it to
a pointer to a `renderer_manager` object (described later).

The other members of the `otc_publisher_callbacks` structure are each callback
functions that are called when events related to the published stream occur:

* `on_stream_created` -- Invoked when the publisher starts streaming
  to the session.

* `on_render_frame` -- Called each time the publisher is ready to render
  a new video frame to the stream.

* `on_stream_destroyed` -- Invoked when the publisher's stream is destroyed.

* `on_error` -- Invoked when an error occurs in publishing the stream.

The application then calls the `otc_publisher_new()`, defined in the OpenTok
Linux SDK, to create an `otc_publisher` structure, which represents the
OpenTok publisher. The `otc_publisher_new()` method takes three arguments:

* `name` -- A name (optional) identifying the publisher of the stream.

* `capturer` -- Use this parameter if you want to provide a custom video capturer.
  If it is set to `null`, as in this sample application, the publisher uses the
  default video capturer. (See the [Custom Video Capturer](../Custom-Video-Capturer))
  sample to see how to implement a custom video capturer.)

* `callbacks` -- The `otc_publisher_callbacks` publisher callback structure,
  described above.


```c
g_publisher = otc_publisher_new("opentok-linux-sdk-samples",
                                nullptr, /* Use WebRTC's video capturer. */
                                &publisher_callbacks);
if (g_publisher == nullptr) {
  std::cout << "Could not create OpenTok publisher successfully" << std::endl;
  otc_session_delete(session);
  return EXIT_FAILURE;
}
```

### Connecting to an OpenTok session

After connecting to the session and creating a publisher instance,
the application calls the `otc_session_connect()` function, defined in 
the OpenTok Linux SDK:

```c
  otc_session_connect(session, TOKEN);
```

This function connects the client to the OpenTok session. It takes two arguments:

* The `otc_session` structure instance.

* The OpenTok token string.

### Publishing a stream to the session

Upon successful connection, the `otc_session_callbacks.on_connected` callback function
is invoked, and the application publishes a stream to the session.

When the application connects to the OpenTok session, the
`on_session_connected()` callback function is called (see the previous
section), and it calls the `otc_session_publish()` function, defined
by the OpenTok Linux SDK, to publish a stream to the OpenTok session:

```c
static void on_session_connected(otc_session *session, void *user_data) {
  std::cout << __FUNCTION__ << " callback function" << std::endl;

  g_is_connected = true;

  if ((session != nullptr) && (g_publisher != nullptr)) {
    if (otc_session_publish(session, g_publisher) == OTC_SUCCESS) {
      g_is_publishing = true;
      return;
    }
    std::cout << "Could not publish successfully" << std::endl;
  }
}
```

the `otc_session_publish()` function takes two arguments:

* The `otc_session` structure.
* The `otc_publisher` structure.

It returns `OTC_SUCCESS` when it successfully starts publishing a stream
to the session.

### Rendering the publisher video

The /common/src/renderer.h defines the interface for the Renderer and RendererManager
C++ classes. The application uses these to render video frames in the user
interface. These classes use the
[SDL library](https://wiki.libsdl.org/SDL_CreateRGBSurfaceFrom)) for rendering.

After the publisher is created (see above), the app calls the `createRenderer()`
method of the RenderManager instance, passing in the `otc_publisher` structure.

```c
renderer_manager.createRenderer(g_publisher);
```

And after the application connects to the session (see above) it calls
the `runEventLoop()` function of the RenderManager:

```c
renderer_manager.runEventLoop();
```

When a new video frame is available in the published stream, the
`on_render_frame()` callback of the `otc_publisher_callbacks`
structure, which is set to the `on_publisher_render_frame()` function, is called:

```c
static void on_publisher_render_frame(otc_publisher *publisher,
                                      void *user_data,
                                      const otc_video_frame *frame) {
  RendererManager *render_manager = static_cast<RendererManager*>(user_data);
  if (render_manager == nullptr) {
    return;
  }
  render_manager->addFrame(publisher, frame);
}
```

The `frame` parameter of
this function is a pointer to an `otc_video_frame` structure. When this callback
is invoked, the `RendererManager.addFrame()` method is called.

```c
void RendererManager::addFrame(void* key, const otc_video_frame *frame) {
  pushEvent(kRendererDraw, key, (void*)otc_video_frame_convert(OTC_VIDEO_FRAME_FORMAT_ARGB32, frame));
}
```

The `otc_video_frame_convert()` function, defined in the OpenTok Linux SDK,
converts the supplied ARGB32 frame to an `otc_video_frame` structure (also
defined in the SDK).

The `RendererManager.handleCustomEvent()` method handles the `kRendererDraw` event.
It grabs the `otc_video_frame` structure, and passes it into the `Renderer.onFrame()`
method: 

```c
otc_video_frame*  frame = (otc_video_frame*)event.user.data2;
renderers_[event.user.data1]->onFrame(frame);
otc_video_frame_delete(frame);
```

The `Renderer::onFrame` function uses the `otc_video_frame_get_plane_binary_data()`
function of the OpenTok Linux SDK to convert the `otc_video_frame`
structure into a buffer containing plane data. Then it calls the
`SDL_CreateRGBSurfaceFrom()` function 
(from the SDL library) to converts the plane data into an SDL_Surface object
and renders the result:

```c
void Renderer::onFrame(otc_video_frame* frame) {
  if (!window_) {
    return;
  }
  SDL_Surface* surface_ =  SDL_GetWindowSurface(window_);

  auto pixels = otc_video_frame_get_plane_binary_data(frame, static_cast<enum otc_video_frame_plane>(0));
  SDL_Surface* sdl_frame = SDL_CreateRGBSurfaceFrom(
      const_cast<unsigned char*>(pixels),
      otc_video_frame_get_width(frame),
      otc_video_frame_get_height(frame),
      32,
      otc_video_frame_get_width(frame) * 4,
      0,0,0,0);

  SDL_BlitScaled(sdl_frame, NULL, surface_, NULL); // blit it to the screen
  SDL_FreeSurface(sdl_frame);

  SDL_UpdateWindowSurface(window_);

}
```

### Subscribing to streams in the session

When the application creates a `otc_session_callbacks` structure (see
[Connecting to an OpenTok session](#connecting-to-an-opentok-session)),
it sets the `on_stream_received()` callback function, which is defined in the
OpenTok Linux SDK.

This function is called when a stream published by another client in the session
is received. This can happen when you connect to an existing session and there are
existing streams in the session or when other clients publish new streams to a
session you have connected to.

In the implementation of this callback function, the application creates
a structure of type`otc_subscriber_callbacks`, defined in the OpenTok Linux SDK:

```c
struct otc_subscriber_callbacks subscriber_callbacks = {0};
subscriber_callbacks.user_data = user_data;
subscriber_callbacks.on_connected = on_subscriber_connected;
subscriber_callbacks.on_render_frame = on_subscriber_render_frame;
subscriber_callbacks.on_error = on_subscriber_error;
```

The `user_data` member of the `subscriber_callbacks` structure is used to
set data associated with the instance. In this application, we set it to
the `user_data` passed into the `on_stream_received()` callback.

The other members of the `otc_subscriber_callbacks` structure are callback functions that
are called when events related to the subscribed stream occur:

* `on_connected` -- Called when the subscriber has has successfully connected
  to the stream and begins playing media.

* `on_render_frame` -- Called when the subscriber has a new video frame to
  render from the stream.

* `on_error` -- Invoked when an error occurs in subscribing to the stream.

Then the code calls the `otc_subscriber_new()` function to instantiate a
`otc_subscriber` struct (defined by the OpenTok Linux SDK), representing the
subscriber:

```
otc_subscriber *subscriber = otc_subscriber_new(stream,
                                                &subscriber_callbacks);
if (subscriber == nullptr) {
  std::cout << "Could not create OpenTok subscriber successfully" << std::endl;
  return;
}
```

The function takes two arguments:

* `stream` -- The `otc_stream` struct for the the stream, obtained from the
  `on_stream_received` callback function.

* `callbacks` -- The `subscriber_callbacks` subscribers callback structure, described
  above.

This function returns an `otc_subscriber` structure, representing the subscriber
to the OpenTok stream.

The application then registers the subscriber with the RenderManager (as it did
with the publisher). Then the application then calls the `otc_session_subscribe()`
function, defined by the OpenTok Linux SDK:

```c
render_manager->createRenderer(subscriber);
if (otc_session_subscribe(session, subscriber) == OTC_SUCCESS) {
  const std::lock_guard<std::mutex> lock(g_subscriber_map_mutex);
  g_subscriber_map[std::string(otc_stream_get_id(stream))] = subscriber;
  return;
}
std::cout << "Could not subscribe successfully" << std::endl;
```

This function returns an `otc_subscriber` struct and starts subscribing to the stream
(starts receiving the audio-video data from the stream).

When a new video frame is available in the subscribed stream, the
`on_subscriber_render_frame()` callback of the `otc_subscriber_callbacks`
structure, which is set to the `on_subscriber_render_frame()` function, is called:

```c
static void on_subscriber_render_frame(otc_subscriber *subscriber,
                                       void *user_data,
                                       const otc_video_frame *frame) {
  RendererManager *render_manager = static_cast<RendererManager*>(user_data);
  if (render_manager == nullptr) {
    return;
  }
  render_manager->addFrame(subscriber, frame);
}
```

The `frame` parameter of ghis function is a pointer to an `otc_video_frame` structure.
When this callback is invoked, the `RendererManager.addFrame()` method is called.
And it renders the frame in the same way it renders a publisher's video frame
(described above).

### Stopping publisher and subscriber streams and disconnecting from the session

When the RenderManager event loop exits, the app calls the following
functions in the OpenTok Linux SDK:

* `otc_session_unsubscribe(session, subscriber)` -- This unsubscribes from
  the subscriber (causing the applcation to stop receiving its audio-video stream).

* `otc_subscriber_delete(subscriber` -- This frees the memory used by the
  `otc_subscriber` instance. The application also sets `subscriber` variable
  to `nullptr`.

* `otc_session_unpublish(session, g_publisher)` -- This causes the application
  to stop publishing the audio-video stream.

* `otc_publisher_delete(g_publisher)` -- This frees the memory used by the
  `otc_publisher` instance. The application also sets `g_publisher` variable
  to `nullptr`.

* `otc_session_disconnect(session)` -- This causes the application
  to disconnect from the OpenTok session (and stop communicating with the
  OpenTok servers).

* `otc_session_delete(session)` -- This frees the memory used by the
  `otc_session` instance. The application also sets `session` variable
  to `nullptr`.

Finally, the app calls `otc_destroy()` to destroy the OpenTok Linux SDK
library engine.

## Next steps

See the [Vonage Video API developer center](https://tokbox.com/developer/)
for more information on the OpenTok Linux SDK.
