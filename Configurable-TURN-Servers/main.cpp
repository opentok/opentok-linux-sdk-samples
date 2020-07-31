#include <opentok.h>

#include <atomic>
#include <cstdlib>
#include <iostream>

#include "config.h"
#include "renderer.h"

#define MAX_CHAR_ARRAY_LENGTH 256

static std::atomic<bool> g_is_connected(false);

static void on_session_connected(otc_session *session, void *user_data) {
  std::cout << __FUNCTION__ << " callback function" << std::endl;

  g_is_connected = true;
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

#ifdef CONSOLE_LOGGING
  otc_log_set_logger_callback(on_otc_log_message);
  otc_log_enable(OTC_LOG_LEVEL_ALL);
#endif

  struct otc_session_callbacks session_callbacks = {0};
  session_callbacks.on_connected = on_session_connected;
  session_callbacks.on_connection_created = on_session_connection_created;
  session_callbacks.on_connection_dropped = on_session_connection_dropped;
  session_callbacks.on_stream_received = on_session_stream_received;
  session_callbacks.on_stream_dropped = on_session_stream_dropped;
  session_callbacks.on_disconnected = on_session_disconnected;
  session_callbacks.on_error = on_session_error;

  // Provide the ICE configuration here.
  struct otc_custom_ice_config ice_config;
  ice_config.num_ice_servers = 1;
  ice_config.ice_url = (char **)malloc(sizeof(char *) * 1);
  ice_config.ice_url[0] = (char *)malloc(sizeof(char) * MAX_CHAR_ARRAY_LENGTH);
  memset(ice_config.ice_url[0], '\0',  MAX_CHAR_ARRAY_LENGTH);
  strcpy(ice_config.ice_url[0], "<ICE_SERVER_URL>");
  ice_config.ice_user = (char **)malloc(sizeof(char *) * 1);
  ice_config.ice_user[0] = (char *)malloc(sizeof(char) * MAX_CHAR_ARRAY_LENGTH);
  memset(ice_config.ice_user[0], '\0',  MAX_CHAR_ARRAY_LENGTH);
  strcpy(ice_config.ice_user[0], "<ICE_SERVER_USER>");
  ice_config.ice_credential = (char **)malloc(sizeof(char *) * 1);
  ice_config.ice_credential[0] = (char *)malloc(sizeof(char) * MAX_CHAR_ARRAY_LENGTH);
  memset(ice_config.ice_credential[0], '\0',  MAX_CHAR_ARRAY_LENGTH);
  strcpy(ice_config.ice_credential[0], "<ICE_SERVER_CREDENTIAL>");
  ice_config.force_turn = OTC_TRUE;
  ice_config.use_custom_turn_only = OTC_FALSE;

  otc_session_settings *session_settings = otc_session_settings_new();
  if (session_settings != nullptr) {
    otc_session_settings_set_custom_ice_config(session_settings,
                                               &ice_config);
  }

  otc_session *session = nullptr;

  session = otc_session_new_with_settings(API_KEY,
                                          SESSION_ID,
                                          &session_callbacks,
                                          session_settings);

  if (session == nullptr) {
    std::cout << "Could not create OpenTok session successfully" << std::endl;
    return EXIT_FAILURE;
  }

  otc_session_connect(session, TOKEN);

  RendererManager renderer_manager;
  renderer_manager.runEventLoop();

  if ((session != nullptr) && g_is_connected.load()) {
    otc_session_disconnect(session);
  }

  if (ice_config.ice_url != nullptr) {
    if (ice_config.ice_url[0] != nullptr) {
      free(ice_config.ice_url[0]);
    }
    free(ice_config.ice_url);
  }

  if (ice_config.ice_user != nullptr) {
    if (ice_config.ice_user[0] != nullptr) {
      free(ice_config.ice_user[0]);
    }
    free(ice_config.ice_user);
  }

  if (ice_config.ice_credential != nullptr) {
    if (ice_config.ice_credential[0] != nullptr) {
      free(ice_config.ice_credential[0]);
    }
    free(ice_config.ice_credential);
  }

  if (session_settings != nullptr) {
    otc_session_settings_delete(session_settings);
  }

  if (session != nullptr) {
    otc_session_delete(session);
  }

  otc_destroy();

  return EXIT_SUCCESS;
}
