#include "conference.h"

#include <cstdlib>
#include <iostream>
#include <string>

#define OPENTOK_PRODUCTION_SERVER_HOST_NAME "api.opentok.com"
#define OPENTOK_PRODUCTION_SERVER_PORT_NUMBER 443
#define SERVER_PATH ""

namespace {

void onOpenTokSubscriberRenderFrameCallback(otc_subscriber *subscriber, void *user_data, const otc_video_frame *frame) {
  Conference *conference = reinterpret_cast<Conference *>(user_data);
  if (conference == nullptr) {
    return;
  }
  std::shared_ptr<RendererManager> rendererManager = conference->getRenderManager();
  if (rendererManager == nullptr) {
    return;
  }
  rendererManager->addFrame(subscriber, frame);
}

void onOpenTokSubscriberErrorCallback(otc_subscriber *subscriber,
                                      void *user_data,
                                      const char* error_string,
                                      enum otc_subscriber_error error_code) {
  Conference *conference = reinterpret_cast<Conference *>(user_data);
  if (conference == nullptr) {
    return;
  }
  conference->onOpenTokSubscriberError(std::string(error_string));
}


void onOpenTokPublisherStreamCreatedCallback(otc_publisher *publisher, 
                                             void *user_data, 
                                             const otc_stream *stream) {
  Conference *conference = reinterpret_cast<Conference *>(user_data);
  if (conference == nullptr) {
    return;
  }
  conference->onOpenTokPublisherStreamCreated();
}

void onOpenTokPublisherRenderFrameCallback(otc_publisher *publisher,
                                             void *user_data,
                                             const otc_video_frame *frame) {
  Conference *conference = reinterpret_cast<Conference *>(user_data);
  if (conference == nullptr) {
    return;
  }
  std::shared_ptr<RendererManager> rendererManager = conference->getRenderManager();
  if (rendererManager == nullptr) {
    return;
  }
  rendererManager->addFrame(publisher, frame);
}

void onOpenTokPublisherErrorCallback(otc_publisher *publisher,
                                     void *user_data,
                                     const char* error_string,
                                     enum otc_publisher_error error_code) {
  Conference *conference = reinterpret_cast<Conference *>(user_data);
  if (conference == nullptr) {
    return;
  }
  conference->onOpenTokPublisherError(std::string(error_string));
}

void onOpenTokSessionConnectedCallback(otc_session *session, void *user_data) {
  Conference *conference = reinterpret_cast<Conference *>(user_data);
  if (conference == nullptr) {
    return;
  }
  conference->onOpenTokSessionConnected();
}

void onOpenTokSessionStreamReceivedCallback(otc_session *session, void *user_data, const otc_stream *stream) {
  Conference *conference = reinterpret_cast<Conference *>(user_data);
  if (conference == nullptr) {
    return;
  }
  std::shared_ptr<RendererManager> rendererManager = conference->getRenderManager();
  if (rendererManager == nullptr) {
    return;
  }

  struct otc_subscriber_cb subscriber_cb = {0};
  subscriber_cb.user_data = conference;
  subscriber_cb.on_render_frame = onOpenTokSubscriberRenderFrameCallback;
  subscriber_cb.on_error = onOpenTokSubscriberErrorCallback;
  otc_subscriber* subscriber = otc_subscriber_new((otc_stream*)stream, &subscriber_cb);
  if (subscriber == nullptr) {
    conference->onOpenTokSubscriberError("Could not create subscriber. Abort.");
    return;
  }
  conference->registerSubscriber(subscriber, stream);
  rendererManager->createRenderer(subscriber);
  otc_session_subscribe(session, subscriber);
  conference->onOpenTokSubscriberCreated();
}

void onOpenTokSessionStreamDroppedCallback(otc_session *session, void *user_data, const otc_stream *stream) {
  Conference *conference = reinterpret_cast<Conference *>(user_data);
  if (conference == nullptr) {
    return;
  }
  std::shared_ptr<RendererManager> rendererManager = conference->getRenderManager();
  if (rendererManager == nullptr) {
    return;
  }
  rendererManager->destroyRenderer(conference->getSubscriber(stream));
}

void onOpenTokSessionErrorCallback(otc_session *session, void *user_data, const char* error_string,
                                   enum otc_session_error error) {
  Conference *conference = reinterpret_cast<Conference *>(user_data);
  if (conference == nullptr) {
    return;
  }
  conference->onOpenTokSessionError(std::string(error_string));
}

} // namespace

Conference::Conference(const std::string& host, const int port, const std::string& name, const std::shared_ptr<ConferenceObserver>& observer) :
  host_(host),
  port_(port),
  name_(name),
  observer_(observer),
  credentialServiceProviderHelper_(std::unique_ptr<CredentialServiceProviderHelper>(new CredentialServiceProviderHelper(this))) {

  init();
}

Conference::Conference(const std::string& name, const std::shared_ptr<ConferenceObserver>& observer) :
  host_(std::string(OPENTOK_PRODUCTION_SERVER_HOST_NAME)),
  port_(OPENTOK_PRODUCTION_SERVER_PORT_NUMBER),
  name_(name),
  observer_(observer),
  credentialServiceProviderHelper_(std::unique_ptr<CredentialServiceProviderHelper>(new CredentialServiceProviderHelper(this))) {

  init();
}

Conference::Conference(const std::string& name, const std::shared_ptr<ConferenceObserver>& observer, const std::shared_ptr<RendererManager>& rendererManager) :
  host_(std::string(OPENTOK_PRODUCTION_SERVER_HOST_NAME)),
  port_(OPENTOK_PRODUCTION_SERVER_PORT_NUMBER),
  name_(name),
  observer_(observer),
  credentialServiceProviderHelper_(std::unique_ptr<CredentialServiceProviderHelper>(new CredentialServiceProviderHelper(this))),
  rendererManager_(rendererManager) {

  init();
}

Conference::~Conference() {
  otc_destroy();
}

void Conference::join(const std::string& apiKey, const std::string& sessionId, const std::string& token) {
  joinImpl(apiKey, sessionId, token);
}

void Conference::join(const std::string& conference, const std::string& credentialServiceProviderURL) {
  credentialServiceProviderHelper_->requestCredentials(conference, credentialServiceProviderURL);
}

void Conference::leave() {
  // Clean up subscribers.
  for (auto const& subscribersStream : subscribersStreamMap_) {
    if (subscribersStream.second != nullptr) {
      otc_subscriber_delete(subscribersStream.second);
    }
  }
  if (publisherStreamCreated_) {
    otc_session_unpublish(session_, publisher_);
    publisherStreamCreated_ = false;
  }
  if (publisher_ != nullptr) {
    otc_publisher_delete(publisher_);
    publisher_ = nullptr;
  }
  if (sessionConnected_) {
    otc_session_disconnect(session_);
    sessionConnected_ = false;
  }
  if (session_ != nullptr) {
    otc_session_delete(session_);
    session_ = nullptr;
  }
}

void Conference::registerSubscriber(otc_subscriber* subscriber, const otc_stream *stream) {
  subscribersStreamMap_[std::string(otc_stream_get_id(stream))] = subscriber;
}

otc_subscriber* Conference::getSubscriber(const otc_stream *stream) {
  return subscribersStreamMap_[std::string(otc_stream_get_id(stream))];
}

void Conference::onOpenTokSessionConnected() {
  sessionConnected_ = true;
  if (observer_ == nullptr) {
    return;
  }
  observer_->onConnected();
  if ((session_ != nullptr) && (publisher_ != nullptr)) {
    otc_session_publish(session_, publisher_);
  }
}

void Conference::onOpenTokSessionError(const std::string& errorMessage) {
  if (observer_ == nullptr) {
    return;
  }
  observer_->onError(errorMessage);
}

void Conference::onOpenTokPublisherStreamCreated() {
  publisherStreamCreated_ = true;
  if (observer_ == nullptr) {
    return;
  }
  observer_->onInCall();
}

void Conference::onOpenTokPublisherError(const std::string& errorMessage) {
  if (observer_ == nullptr) {
    return;
  }
  observer_->onError(errorMessage);
}

void Conference::onOpenTokSubscriberCreated() {
  if (observer_ == nullptr) {
    return;
  }
  observer_->onNewRemoteParticipant();
}

void Conference::onOpenTokSubscriberError(const std::string& errorMessage) {
  if (observer_ == nullptr) {
    return;
  }
  observer_->onError(errorMessage);
}

void Conference::onCredentialsReady(const std::string& apiKey,
                                    const std::string& sessionId,
                                    const std::string& token) {
  joinImpl(apiKey, sessionId, token);
}

void Conference::onError(const std::string& errorMessage) {
  if (observer_ == nullptr) {
    return;
  }
  observer_->onError(errorMessage);
}

std::shared_ptr<RendererManager> Conference::getRenderManager() {
  return rendererManager_;
}

void Conference::init() {
  if (otc_init(nullptr) != OTC_SUCCESS) {
    if (observer_ == nullptr) {
      return;
    }
    observer_->onError("Could not initialize. Abort.");
    return;
  }
#ifdef DEBUG
  otc_log_enable(OTC_LOG_LEVEL_ALL);
#else
  otc_log_enable(OTC_LOG_LEVEL_DISABLED);
#endif
}

void Conference::joinImpl(const std::string& apiKey, const std::string& sessionId, const std::string& token) {
  struct otc_session_cb session_callbacks = {0};
  session_callbacks.user_data = this;
  session_callbacks.on_connected = onOpenTokSessionConnectedCallback;
  session_callbacks.on_stream_received = onOpenTokSessionStreamReceivedCallback;
  session_callbacks.on_stream_dropped = onOpenTokSessionStreamDroppedCallback;
  session_callbacks.on_error = onOpenTokSessionErrorCallback;
  session_ = otc_session_new(apiKey.c_str(), sessionId.c_str(), &session_callbacks);

  if (session_ == nullptr) {
    if (observer_ == nullptr) {
      return;
    }
    observer_->onError("Could not create session. Abort.");
    return;
  }

  otc_session_connect_to_host(session_,
                              host_.c_str(),
                              SERVER_PATH,
                              port_,
                              OTC_TRUE,
                              token.c_str());

  struct otc_publisher_cb publisher_cb = {0};
  publisher_cb.user_data = this;
  publisher_cb.on_stream_created = onOpenTokPublisherStreamCreatedCallback;
  publisher_cb.on_render_frame = onOpenTokPublisherRenderFrameCallback;
  publisher_cb.on_error = onOpenTokPublisherErrorCallback;
  publisher_ = otc_publisher_new(name_.c_str(), nullptr, &publisher_cb);

  if (publisher_ == nullptr) {
    otc_session_delete(session_);
    if (observer_ == nullptr) {
      return;
    }
    observer_->onError("Could not create publisher. Abort.");
    return;
  }

  if (rendererManager_ == nullptr) {
    return;
  }
  rendererManager_->createRenderer(publisher_);
}
