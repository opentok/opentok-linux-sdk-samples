#include <opentok.h>

#include <atomic>
#include <cstdlib>
#include <iostream>

#include "config.h"
#include "otk_thread.h"
#include "renderer.h"

static std::atomic<bool> g_is_connected(false);
static otc_publisher *g_publisher = nullptr;
static std::atomic<bool> g_is_publishing(false);

struct audio_device {
  otc_audio_device_callbacks audio_device_callbacks;
  otk_thread_t capturer_thread;
  std::atomic<bool> capturer_thread_exit;
};

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

static otc_bool audio_device_destroy_capturer(const otc_audio_device *audio_device,
                                              void *user_data) {
  struct audio_device *device = static_cast<struct audio_device *>(user_data);
  if (device == nullptr) {
    return OTC_FALSE;
  }

  device->capturer_thread_exit = true;
  otk_thread_join(device->capturer_thread);

  return OTC_TRUE;
}

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

static void on_session_connection_created(otc_session *session,
                                          void *user_data,
                                          const otc_connection *connection) {
  std::cout << __FUNCTION__ << " callback function" << std::endl;
}

static void on_session_connection_dropped(otc_session *session,
                                          void *user_data,
                                          const otc_connection *connection) {
  std::cout << __FUNCTION__ << " callback function" << std::endl;
}

static void on_session_stream_received(otc_session *session,
                                       void *user_data,
                                       const otc_stream *stream) {
  std::cout << __FUNCTION__ << " callback function" << std::endl;
}

static void on_session_stream_dropped(otc_session *session,
                                      void *user_data,
                                      const otc_stream *stream) {
  std::cout << __FUNCTION__ << " callback function" << std::endl;
}

static void on_session_disconnected(otc_session *session, void *user_data) {
  std::cout << __FUNCTION__ << " callback function" << std::endl;
}

static void on_session_error(otc_session *session,
                             void *user_data,
                             const char *error_string,
                             enum otc_session_error_code error) {
  std::cout << __FUNCTION__ << " callback function" << std::endl;
  std::cout << "Session error. Error : " << error_string << std::endl;
}

static void on_publisher_stream_created(otc_publisher *publisher,
                                        void *user_data,
                                        const otc_stream *stream) {
  std::cout << __FUNCTION__ << " callback function" << std::endl;
}

static void on_publisher_render_frame(otc_publisher *publisher,
                                      void *user_data,
                                      const otc_video_frame *frame) {
  RendererManager *render_manager = static_cast<RendererManager*>(user_data);
  if (render_manager == nullptr) {
    return;
  }
  render_manager->addFrame(publisher, frame);
}

static void on_publisher_stream_destroyed(otc_publisher *publisher,
                                          void *user_data,
                                          const otc_stream *stream) {
  std::cout << __FUNCTION__ << " callback function" << std::endl;
}

static void on_publisher_error(otc_publisher *publisher,
                               void *user_data,
                               const char* error_string,
                               enum otc_publisher_error_code error_code) {
  std::cout << __FUNCTION__ << " callback function" << std::endl;
  std::cout << "Publisher error. Error code: " << error_string << std::endl;
}

static void on_otc_log_message(const char* message) {
  std::cout <<  __FUNCTION__ << ":" << message << std::endl;
}

int main(int argc, char** argv) {
  if (otc_init(nullptr) != OTC_SUCCESS) {
    std::cout << "Could not init OpenTok library" << std::endl;
    return EXIT_FAILURE;
  }

#ifdef CONSOLE_LOGGING
  otc_log_set_logger_callback(on_otc_log_message);
  otc_log_enable(OTC_LOG_LEVEL_ALL);
#endif

  struct audio_device *device = (struct audio_device *)malloc(sizeof(struct audio_device));
  device->audio_device_callbacks = {0};
  device->audio_device_callbacks.user_data = static_cast<void *>(device);
  device->audio_device_callbacks.destroy_capturer = audio_device_destroy_capturer;
  device->audio_device_callbacks.start_capturer = audio_device_start_capturer;
  device->audio_device_callbacks.get_capture_settings = audio_device_get_capture_settings;
  otc_set_audio_device(&(device->audio_device_callbacks));
  
  struct otc_session_callbacks session_callbacks = {0};
  session_callbacks.on_connected = on_session_connected;
  session_callbacks.on_connection_created = on_session_connection_created;
  session_callbacks.on_connection_dropped = on_session_connection_dropped;
  session_callbacks.on_stream_received = on_session_stream_received;
  session_callbacks.on_stream_dropped = on_session_stream_dropped;
  session_callbacks.on_disconnected = on_session_disconnected;
  session_callbacks.on_error = on_session_error;

  otc_session *session = nullptr;
  session = otc_session_new(API_KEY, SESSION_ID, &session_callbacks);

  if (session == nullptr) {
    std::cout << "Could not create OpenTok session successfully" << std::endl;
    return EXIT_FAILURE;
  }

  RendererManager renderer_manager;
  struct otc_publisher_callbacks publisher_callbacks = {0};
  publisher_callbacks.user_data = &renderer_manager;
  publisher_callbacks.on_stream_created = on_publisher_stream_created;
  publisher_callbacks.on_render_frame = on_publisher_render_frame;
  publisher_callbacks.on_stream_destroyed = on_publisher_stream_destroyed;
  publisher_callbacks.on_error = on_publisher_error;

  g_publisher = otc_publisher_new("opentok-linux-sdk-samples",
                                  nullptr, /* Use default video capturer. */
                                  &publisher_callbacks);
  if (g_publisher == nullptr) {
    std::cout << "Could not create OpenTok publisher successfully" << std::endl;
    otc_session_delete(session);
    return EXIT_FAILURE;
  }
  renderer_manager.createRenderer(g_publisher);

  otc_session_connect(session, TOKEN);

  renderer_manager.runEventLoop();

  renderer_manager.destroyRenderer(g_publisher);

  if ((session != nullptr) && (g_publisher != nullptr) && g_is_publishing.load()) {
    otc_session_unpublish(session, g_publisher);
  }

  if (g_publisher != nullptr) {
    otc_publisher_delete(g_publisher);
  }

  if ((session != nullptr) && g_is_connected.load()) {
    otc_session_disconnect(session);
  }

  if (session != nullptr) {
    otc_session_delete(session);
  }

  if (device != nullptr) {
    free(device);
  }

  otc_destroy();

  return EXIT_SUCCESS;
}
