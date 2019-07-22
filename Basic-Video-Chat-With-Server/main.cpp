#include <cstdlib>
#include <iostream>
#include <string>

#include "conference.h"
#include "renderer.h"

#define PUBLISHER_NAME "OpenTok Linux SDK Sample - Basic Video Chat With Server"

#define SERVER_URL "http://localhost:8080"

struct RoomInfo {
  std::string name_;

  bool isValid() const {
    return !name_.empty();
  }
};

class Arguments {
 public:
  static bool parse(int argc, char* argv[], RoomInfo* info) {
    for (int i = 1; (i + 1) < argc; i = i + 2) {
      std::string key(argv[i]);
      std::string param(argv[i + 1]);
      if ((key == "-r") || (key == "--room")) info->name_ = param;
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
    std::cout << "MyConferenceObserver::onNewRemoteParticipant()" << std::endl;
  }

  void onError(const std::string& errorMessage) {
    std::cout << "MyConferenceObserver::onError() - " << errorMessage << std::endl;
  }
};

int main(int argc, char* argv[]) {
  RoomInfo roomInfo;
  if (!Arguments::parse(argc, argv, &roomInfo)) {
    Arguments::printUsage();
    return EXIT_FAILURE;
  }

  std::shared_ptr<MyConferenceObserver> observer = std::make_shared<MyConferenceObserver>();
  std::shared_ptr<RendererManager> rendererManager = std::make_shared<RendererManager>();
  std::unique_ptr<Conference> conference(new Conference(std::string(PUBLISHER_NAME), observer, rendererManager));
  conference->join(roomInfo.name_, std::string(SERVER_URL));

  rendererManager->runEventLoop();

  conference->leave();

  std::cout << "Quiting..." << std::endl;

  return EXIT_SUCCESS;
}
