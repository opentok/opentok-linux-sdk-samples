#include <cstdlib>
#include <iostream>
#include <string>

#include "conference.h"
#include "renderer.h"

#define SIGBREAK 21

#define PUBLISHER_NAME "OpenTok Linux SDK Sample - Basic Video Chat"

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
  /* Replace these with your OpenTok API key, an OpenTok session ID, and a token for the session: */
  std::string apiKey = "";
  std::string sessionId = "";
  std::string token = "";

  std::shared_ptr<MyConferenceObserver> observer = std::make_shared<MyConferenceObserver>();
  std::shared_ptr<RendererManager> rendererManager = std::make_shared<RendererManager>();
  std::unique_ptr<Conference> conference(new Conference(std::string(PUBLISHER_NAME), observer, rendererManager));
  conference->join(apiKey, sessionId, token);

  rendererManager->runEventLoop();

  conference->leave();

  std::cout << "Quitting..." << std::endl;

  return EXIT_SUCCESS;
}
