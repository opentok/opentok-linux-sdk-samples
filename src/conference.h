#ifndef CONFERENCE
#define CONFERENCE

#include <cstdlib>
#include <iostream>
#include <string>
#include <unordered_map>

#include "opentok.h"
#include "opentok/internal.h"

#include "credential_service_provider_helper.h"
#include "renderer.h"

struct ConferenceObserver {
  virtual void onConnected() = 0;
  virtual void onInCall() = 0;
  virtual void onNewRemoteParticipant() = 0;
  virtual void onError(const std::string& errorMessage) = 0;
};

class Conference : public CredentialServiceProviderHelperObserver {
 public:
  Conference(const std::string& host, const int port, const std::string& name, const std::shared_ptr<ConferenceObserver>& observer);
  Conference(const std::string& name, const std::shared_ptr<ConferenceObserver>& observer);
  Conference(const std::string& name, const std::shared_ptr<ConferenceObserver>& observer, const std::shared_ptr<RendererManager>& rendererManager);
  ~Conference();

  void join(const std::string& apiKey, const std::string& sessionId, const std::string& token);
  void join(const std::string& conference, const std::string& credentialServiceProviderURL);
  void leave();

  void registerSubscriber(otc_subscriber* subscriber, const otc_stream *stream);
  otc_subscriber* getSubscriber(const otc_stream *stream);
  
  void onOpenTokSessionConnected();
  void onOpenTokSessionError(const std::string& errorMessage);
  void onOpenTokPublisherStreamCreated();
  void onOpenTokPublisherError(const std::string& errorMessage);
  void onOpenTokSubscriberCreated();
  void onOpenTokSubscriberError(const std::string& errorMessage);

  // CredentialServiceProviderHelperObserver implementation.
  void onCredentialsReady(const std::string& apiKey,
                          const std::string& sessionId,
                          const std::string& token) override;
  void onError(const std::string& errorMessage) override;

  // Getters
  std::shared_ptr<RendererManager> getRenderManager();
  
 private:
  void init();
  void joinImpl(const std::string& apiKey, const std::string& sessionId, const std::string& token);
  
  std::string host_;
  int port_;
  std::string name_;
  std::shared_ptr<ConferenceObserver> observer_;
  std::shared_ptr<RendererManager> rendererManager_;
  std::unique_ptr<CredentialServiceProviderHelper> credentialServiceProviderHelper_;
  otc_session *session_ = nullptr;
  otc_publisher* publisher_ = nullptr;
  bool sessionConnected_ = false;
  bool publisherStreamCreated_ = false;
  std::unordered_map<std::string, otc_subscriber*> subscribersStreamMap_;
};

#endif // CONFERENCE
