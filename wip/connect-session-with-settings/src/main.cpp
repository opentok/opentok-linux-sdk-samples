#include <opentok.h>

#include <cstdlib>
#include <iostream>

#include "renderer.h"

#define API_KEY ""
#define SESSION_ID ""
#define TOKEN ""

static otc_bool g_is_connected = OTC_FALSE;

static void on_session_connected(otc_session *session, void *user_data) {
  std::cout << __FUNCTION__ << " callback function" << std::endl;

  g_is_connected = OTC_TRUE;
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
  std::cout << "Session error. Error is: " << error_string << std::endl;
}

static void on_otc_log_message(const char* message) {
  std::cout <<  __FUNCTION__ << ":" << message << std::endl;
}

int main(int argc, char** argv) {
  if (otc_init(nullptr) != OTC_SUCCESS) {
    std::cout << "Could not init OpenTok library" << std::endl;
    return EXIT_FAILURE;
  }
  otc_log_set_logger_func(on_otc_log_message);
  otc_log_enable(OTC_LOG_LEVEL_INFO);

  struct otc_session_callbacks session_callbacks = {0};
  session_callbacks.on_connected = on_session_connected;
  session_callbacks.on_stream_received = on_session_stream_received;
  session_callbacks.on_stream_dropped = on_session_stream_dropped;
  session_callbacks.on_disconnected = on_session_disconnected;
  session_callbacks.on_error = on_session_error;

  otc_session_settings *session_settings = otc_session_settings_new();
  if (session_settings == nullptr) {
    std::cout << "Could not create session settings successfuly" << std::endl;
    return EXIT_FAILURE;
  }
  otc_session_settings_set_connection_events_suppressed(session_settings, OTC_TRUE);

  otc_session *session = nullptr;
  session = otc_session_new_with_settings(API_KEY,
                                          SESSION_ID,
                                          &session_callbacks,
                                          session_settings);

  if (session == nullptr) {
    std::cout << "Could not create OpenTok session successfuly" << std::endl;
    return EXIT_FAILURE;
  }

  otc_session_settings_delete(session_settings);

  otc_session_connect(session, TOKEN);

  RendererManager renderer_manager;
  renderer_manager.runEventLoop();

  if ((session != nullptr) && (g_is_connected == OTC_TRUE)) {
    otc_session_disconnect(session);
    g_is_connected = OTC_FALSE;
  }

  if (session != nullptr) {
    otc_session_delete(session);
  }

  otc_destroy();

  return EXIT_SUCCESS;
}
