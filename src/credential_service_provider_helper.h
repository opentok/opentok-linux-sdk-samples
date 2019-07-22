#ifndef CREDENTIAL_SERVICE_PROVIDER_HELPER
#define CREDENTIAL_SERVICE_PROVIDER_HELPER

#include <string>

#include <curl/curl.h>

struct CredentialServiceProviderResponse {
  char* payload;
  size_t size;
};

struct CredentialServiceProviderHelperObserver {
  virtual void onCredentialsReady(const std::string& apiKey,
                                  const std::string& sessionId,
                                  const std::string& token) = 0;
  virtual void onError(const std::string& errorMessage) = 0;
};

class CredentialServiceProviderHelper {
 public:
  CredentialServiceProviderHelper(CredentialServiceProviderHelperObserver* observer);
  ~CredentialServiceProviderHelper();

  void requestCredentials(const std::string& conference,
                          const std::string& credentialServiceProviderURL);

 private:
  void init();
  void destroy();

  CredentialServiceProviderHelperObserver* observer_ = nullptr;
  CURL* curlHandle_ = nullptr;
  struct curl_slist* curlHeaders_ = nullptr;
  CredentialServiceProviderResponse response_;
};

#endif // CREDENTIAL_SERVICE_PROVIDER_HELPER
