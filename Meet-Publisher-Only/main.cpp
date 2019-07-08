#include <cstdlib>
#include <iostream>
#include <string>

#include <uv.h>

#include "conference.h"

#define SIGBREAK 21

#define OPENTOK_TESTING_SERVER_HOST_NAME "anvil-tbdev.opentok.com"
#define OPENTOK_TESTING_SERVER_PORT_NUMBER 443
#define PUBLISHER_NAME "OpenTok Linux SDK Sample - Meet Publisher Only"

#define MEET_SERVER_URL "https://meet.tokbox.com"

struct MeetRoomInfo {
  std::string meetRoom_;

  bool isValid() const {
    return !meetRoom_.empty();
  }
};

class Arguments {
 public:
  static bool parse(int argc, char* argv[], MeetRoomInfo* info) {
    for (int i = 1; (i + 1) < argc; i = i + 2) {
      std::string key(argv[i]);
      std::string param(argv[i + 1]);
      if ((key == "-r") || (key == "--room")) info->meetRoom_ = param;
    }
    return info->isValid();
  }

  static void printUsage() {
    std::cout << "Usage: {-r|--room} <room>" << std::endl;
  }
};

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
  MeetRoomInfo meetRoomInfo;
  if (!Arguments::parse(argc, argv, &meetRoomInfo)) {
    Arguments::printUsage();
    return EXIT_FAILURE;
  }

  uv_signal_t uvSignal;

  std::shared_ptr<MyConferenceObserver> observer = std::make_shared<MyConferenceObserver>();
  std::unique_ptr<Conference> conference(new Conference(std::string(OPENTOK_TESTING_SERVER_HOST_NAME),
                                                        OPENTOK_TESTING_SERVER_PORT_NUMBER,
                                                        std::string(PUBLISHER_NAME),
                                                        observer));
  conference->join(meetRoomInfo.meetRoom_, std::string(MEET_SERVER_URL));

  uv_signal_init(uv_default_loop(), &uvSignal);
  uv_signal_start(&uvSignal, uvSignalCallback, SIGBREAK);

  uv_run(uv_default_loop(), UV_RUN_DEFAULT);
  uv_loop_delete(uv_default_loop());

  conference->leave();

  std::cout << "Quiting..." << std::endl;

  return EXIT_SUCCESS;
}
