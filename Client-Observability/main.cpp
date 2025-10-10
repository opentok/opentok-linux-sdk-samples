#include <opentok.h>

#include <algorithm>
#include <atomic>
#include <cstdlib>
#include <iostream>
#include <map>
#include <mutex>
#include <string>

#include "config.h"
#include "renderer.h"

static std::atomic<bool> g_is_connected(false);
static otc_publisher *g_publisher = nullptr;
static std::atomic<bool> g_is_publishing(false);
static std::map<std::string, void*> g_subscriber_map;
static std::mutex g_subscriber_map_mutex;

static void on_subscriber_connected(otc_subscriber *subscriber,
                                    void *user_data,
                                    const otc_stream *stream) {
  std::cout << __FUNCTION__ << " callback function" << std::endl;
}

static void on_subscriber_render_frame(otc_subscriber *subscriber,
                                       void *user_data,
                                       const otc_video_frame *frame) {
  RendererManager *render_manager = static_cast<RendererManager*>(user_data);
  if (render_manager == nullptr) {
    return;
  }
  render_manager->addFrame(subscriber, frame);
}

static void on_subscriber_error(otc_subscriber* subscriber,
                                void *user_data,
                                const char* error_string,
                                enum otc_subscriber_error_code error) {
  std::cout << __FUNCTION__ << " callback function" << std::endl;
  std::cout << "Subscriber error. Error code: " << error_string << std::endl;
}

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

static void on_subscriber_audio_stats(otc_subscriber *subscriber,
                                      void *user_data,
                                      struct otc_subscriber_audio_stats audio_stats)
{
    std::cout << __FUNCTION__ << " callback function" << std::endl;
    std::cout << "Subscriber audio level: " << audio_stats.audio_level << std::endl;
    std::cout << "Subscriber audio bytes received: " << audio_stats.bytes_received << std::endl;
    std::cout << "Subscriber audio packets received: " << audio_stats.packets_received << std::endl;
    std::cout << "Subscriber audio packets lost: " << audio_stats.packets_lost << std::endl;
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
  RendererManager *render_manager = static_cast<RendererManager*>(user_data);
  if (render_manager == nullptr) {
    return;
  }

  struct otc_subscriber_callbacks subscriber_callbacks = {0};
  subscriber_callbacks.user_data = user_data;
  subscriber_callbacks.on_connected = on_subscriber_connected;
  subscriber_callbacks.on_render_frame = on_subscriber_render_frame;
  subscriber_callbacks.on_error = on_subscriber_error;
  subscriber_callbacks.on_video_stats = on_subscriber_video_stats;
  subscriber_callbacks.on_audio_stats = on_subscriber_audio_stats;

  otc_subscriber *subscriber = otc_subscriber_new(stream,
                                                  &subscriber_callbacks);
  if (subscriber == nullptr) {
    std::cout << "Could not create OpenTok subscriber successfully" << std::endl;
    return;
  }
  render_manager->createRenderer(subscriber);
  if (otc_session_subscribe(session, subscriber) == OTC_SUCCESS) {
    const std::lock_guard<std::mutex> lock(g_subscriber_map_mutex);
    g_subscriber_map[std::string(otc_stream_get_id(stream))] = subscriber;
    return;
  }
  std::cout << "Could not subscribe successfully" << std::endl;
}

static void on_session_stream_dropped(otc_session *session,
                                      void *user_data,
                                      const otc_stream *stream) {
  std::cout << __FUNCTION__ << " callback function" << std::endl;
  otc_subscriber *subscriber = static_cast<otc_subscriber *>(g_subscriber_map[std::string(otc_stream_get_id(stream))]);
  if (subscriber == nullptr) {
    return;
  }
  
  RendererManager *render_manager = static_cast<RendererManager*>(user_data);
  if (render_manager != nullptr) {
    render_manager->destroyRenderer(subscriber);
  }
  otc_subscriber_delete(subscriber);
  const std::lock_guard<std::mutex> lock(g_subscriber_map_mutex);
  g_subscriber_map[std::string(otc_stream_get_id(stream))] = nullptr;
  subscriber = nullptr;
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

void on_publisher_audio_stats(otc_publisher *publisher,
                              void *user_data,
                              struct otc_publisher_audio_stats audio_stats[],
                              size_t number_of_stats)
{
    std::cout << __FUNCTION__ << " callback function" << std::endl;
    // Assume routed, print only statistics for one link
    std::cout << "Publisher audio level: " << audio_stats[0].audio_level << std::endl;
    std::cout << "Publisher audio bytes sent: " << audio_stats[0].bytes_sent << std::endl;
    std::cout << "Publisher audio packets sent: " << audio_stats[0].packets_sent << std::endl;
}

void on_publisher_video_stats(otc_publisher *publisher,
                              void *user_data,
                              struct otc_publisher_video_stats audio_stats[],
                              size_t number_of_stats)
{
    std::cout << __FUNCTION__ << " callback function" << std::endl;
    // Assume routed, print only statistics for one link
    std::cout << "Publisher video bytes sent: " << audio_stats[0].bytes_sent << std::endl;
    std::cout << "Publisher video packets sent: " << audio_stats[0].packets_sent << std::endl;
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

  RendererManager renderer_manager;

  struct otc_session_callbacks session_callbacks = {0};
  session_callbacks.user_data = &renderer_manager;
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

  struct otc_publisher_callbacks publisher_callbacks = {0};
  publisher_callbacks.user_data = &renderer_manager;
  publisher_callbacks.on_stream_created = on_publisher_stream_created;
  publisher_callbacks.on_render_frame = on_publisher_render_frame;
  publisher_callbacks.on_stream_destroyed = on_publisher_stream_destroyed;
  publisher_callbacks.on_error = on_publisher_error;
  publisher_callbacks.on_audio_stats = on_publisher_audio_stats;
  publisher_callbacks.on_video_stats = on_publisher_video_stats;

  // Enable sender-side statistics
  otc_publisher_settings *settings = otc_publisher_settings_new();
  otc_publisher_settings_set_sender_stats_track(settings, OTC_TRUE);
  otc_publisher_settings_set_name(settings, "opentok-linux-sdk-samples");

  g_publisher = otc_publisher_new_with_settings(&publisher_callbacks, settings);

  otc_publisher_settings_delete(settings);

  if (g_publisher == nullptr) {
      std::cout << "Could not create OpenTok publisher successfully" << std::endl;
      otc_session_delete(session);
      return EXIT_FAILURE;
  }
  renderer_manager.createRenderer(g_publisher);

  otc_session_connect(session, TOKEN);

  renderer_manager.runEventLoop();

  if (session != nullptr) {
    const std::lock_guard<std::mutex> lock(g_subscriber_map_mutex);
    std::for_each(g_subscriber_map.begin(),
                  g_subscriber_map.end(),
                  [&renderer_manager, session](std::pair<std::string, void*> element) {
      otc_subscriber *subscriber = static_cast<otc_subscriber *>(element.second);
      if (subscriber == nullptr) {
        return;
      }
      renderer_manager.destroyRenderer(subscriber);
      otc_session_unsubscribe(session, subscriber);
      otc_subscriber_delete(subscriber);
      subscriber = nullptr;
    });
  }

  renderer_manager.destroyRenderer(g_publisher);

  if ((session != nullptr) && (g_publisher != nullptr) && g_is_publishing.load()) {
    otc_session_unpublish(session, g_publisher);
  }

  if (g_publisher != nullptr) {
    otc_publisher_delete(g_publisher);
    g_publisher = nullptr;
  }

  if ((session != nullptr) && g_is_connected.load()) {
    otc_session_disconnect(session);
  }

  if (session != nullptr) {
    otc_session_delete(session);
    session = nullptr;
  }

  otc_destroy();

  return EXIT_SUCCESS;
}
