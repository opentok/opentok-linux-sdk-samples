#include "credential_service_provider_helper.h"

#include <cstdlib>
#include <iostream>
#include <string>

#include <curl/curl.h>
#include <jansson.h>

namespace {

size_t curlWriteFunction(void* contents, size_t size, size_t nmemb, void* userp) {
  size_t realsize = size * nmemb;
  CredentialServiceProviderResponse* response = static_cast<CredentialServiceProviderResponse*>(userp);

  response->payload = (char*)realloc(response->payload, response->size + realsize + 1);
  if (response->payload == nullptr) {
    free(response->payload);
    return -1;
  }

  memcpy(&(response->payload[response->size]), contents, realsize);
  response->size += realsize;
  response->payload[response->size] = 0;

  return realsize;
}

} // namespace

CredentialServiceProviderHelper::CredentialServiceProviderHelper(CredentialServiceProviderHelperObserver* observer) :
    observer_(observer) {
  init();
}

CredentialServiceProviderHelper::~CredentialServiceProviderHelper() {
  destroy();
}

void CredentialServiceProviderHelper::requestCredentials(const std::string& conference,
                                                         const std::string& credentialServiceProviderURL) {
  CURLcode curlReturnCode;

  if (curlHandle_ == nullptr) {
    if (observer_ == nullptr) {
      return;
    }
    observer_->onError("Failed to perform request");
    return;
  }

  if (credentialServiceProviderURL.empty()) {
    if (observer_ == nullptr) {
      return;
    }
    observer_->onError("Failed to perform request");
    return;
  }
#ifdef MEET_PUBLISHER_ONLY
  std::string url = credentialServiceProviderURL + std::string("/") + conference;
#elif OPENTOKDEMO_BASIC_VIDEO_CHAT
  std::string url = credentialServiceProviderURL + std::string("/room/") + conference + std::string("/info");
#else
  std::string url;
  assert(false && "Trying to use the Credential Service Provider helper without an unknown server.");
#endif
  curl_easy_setopt(curlHandle_, CURLOPT_URL, url.c_str());

  curlReturnCode = curl_easy_perform(curlHandle_);
  if ((curlReturnCode != CURLE_OK) || (response_.size < 1)) {
    if (observer_ == nullptr) {
      return;
    }
    observer_->onError("Failed to perform request. Error is " +
                       std::string(curl_easy_strerror(curlReturnCode)));
    return;
  }
  if (response_.payload == nullptr) {
    if (observer_ == nullptr) {
      return;
    }
    observer_->onError("Failed to perform request. No response payload");
    return;
  }

  if (observer_ == nullptr) {
    return;
  }

  json_t* root;
  json_error_t error;

  root = json_loads(response_.payload, JSON_DECODE_ANY, &error);
  if (root == nullptr) {
    if (observer_ == nullptr) {
      return;
    }
    observer_->onError("Failed to parse response");
    return;
  }

  char* rootString = json_dumps(root, JSON_DECODE_ANY);
  free(rootString);

  json_t* obj = json_object_get(root, "apiKey");
  if (obj == nullptr) {
    if (observer_ == nullptr) {
      return;
    }
    observer_->onError("Failed to parse response");
    return;
  }
  const char* apiKey = json_string_value(obj);

  obj = json_object_get(root, "sessionId");
  if (obj == nullptr) {
    if (observer_ == nullptr) {
      return;
    }
    observer_->onError("Failed to parse response");
    return;
  }
  const char* sessionId = json_string_value(obj);

  obj = json_object_get(root, "token");
  if (obj == nullptr) {
    if (observer_ == nullptr) {
      return;
    }
    observer_->onError("Failed to parse response");
    return;
  }
  const char* token = json_string_value(obj);

  observer_->onCredentialsReady(apiKey != nullptr ? std::string(apiKey) : "",
                                sessionId != nullptr ? std::string(sessionId) : "",
                                token != nullptr ? std::string(token) : "");
  json_decref(obj);
  json_decref(root);
}

void CredentialServiceProviderHelper::init() {
  if ((curlHandle_ = curl_easy_init()) == nullptr) {
    if (observer_ == nullptr) {
      return;
    }
    observer_->onError("Failed to init helper");
    return;
  }
  curlHeaders_ = curl_slist_append(curlHeaders_,
                                   "Accept: application/json, text/plain, */* ");

  curl_easy_setopt(curlHandle_, CURLOPT_CUSTOMREQUEST, "GET");

  curl_easy_setopt(curlHandle_, CURLOPT_HTTPHEADER, curlHeaders_);

  curl_easy_setopt(curlHandle_, CURLOPT_WRITEFUNCTION, curlWriteFunction);

  response_.size = 0;
  response_.payload = (char*)calloc(1, sizeof(response_.payload));
  if (response_.payload == nullptr) {
    if (observer_ == nullptr) {
      return;
    }
    observer_->onError("Failed to init helper");
    return;
  }

  curl_easy_setopt(curlHandle_, CURLOPT_WRITEDATA, (void *)&response_);

  curl_easy_setopt(curlHandle_, CURLOPT_USERAGENT, "libcurl-agent/1.0");

  curl_easy_setopt(curlHandle_, CURLOPT_TIMEOUT, 5);

  curl_easy_setopt(curlHandle_, CURLOPT_FOLLOWLOCATION, 1);

  curl_easy_setopt(curlHandle_, CURLOPT_MAXREDIRS, 1);
}

void CredentialServiceProviderHelper::destroy() {
  free(response_.payload);
  curl_slist_free_all(curlHeaders_);
  curlHeaders_ = nullptr;
  curl_easy_cleanup(curlHandle_);
  curlHandle_ = nullptr;
}
