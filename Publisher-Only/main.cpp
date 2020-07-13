#include <cstdlib>
#include <iostream>
#include <string>

#include <uv.h>

#include "conference.h"

#define SIGBREAK 21

#define PUBLISHER_NAME "OpenTok Linux SDK Sample - Publisher Only"

struct MyConferenceObserver: public ConferenceObserver {
  void onConnected() {
    std::cout << "MyConferenceObserver::onConnected()" << std::endl;
  }

  void onInCall() {
    std::cout << "MyConferenceObserver::onInCall()" << std::endl;
  }

  void onNewRemoteParticipant() {
  }

  void onError(const std::string& errorMessage) {
    std::cout << "MyConferenceObserver::onError() - " << errorMessage << std::endl;
  }
};

static void uvSignalCallback(uv_signal_t* handle, int signum) {
  uv_signal_stop(handle);
}

int main(int argc, char* argv[]) {
  /* Replace these with your OpenTok API key, an OpenTok session ID, and a token for the session: */
  std::string apiKey = "";
  std::string sessionId = "";
  std::string token = "";

  uv_signal_t uvSignal;

  std::shared_ptr<MyConferenceObserver> observer = std::make_shared<MyConferenceObserver>();
  std::unique_ptr<Conference> conference(new Conference(std::string(PUBLISHER_NAME), observer));
  conference->join(apiKey, sessionId, token);

  uv_signal_init(uv_default_loop(), &uvSignal);
  uv_signal_start(&uvSignal, uvSignalCallback, SIGBREAK);

  uv_run(uv_default_loop(), UV_RUN_DEFAULT);
  uv_loop_delete(uv_default_loop());

  conference->leave();

  std::cout << "Quitting..." << std::endl;

  return EXIT_SUCCESS;
}
