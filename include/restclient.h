#ifndef _INCLUDE_RESTCLIENT_H
#define _INCLUDE_RESTCLIENT_H
#include <atomic>
#include <cstdint>
#include <string>
#include <optional>
#include <unordered_map>
#include <vector>
#include <expected>
#include <string_view>
class NyaBot;

enum class RestErrors {
  TooLarge,
  NotFound,
  Forbidden,
  IOERR
};

struct RateLimit {
  std::string bucket;
  int resetAfter;
  int reset;
};

struct RateLimitTables {
  std::unordered_map<std::string, RateLimit> userRateLimitTable;
  std::atomic<bool> globalLimit;
  std::atomic<int> globalResetAfter;
};

class RestClient {
public:
  RestClient(NyaBot *bot);
  /// @brief send a get request to a discord api endpoint
  /// @param endpoint endpoint to send the request to
  std::expected<std::string, RestErrors> get(const std::string& endpoint);

  /// @brief send a post request to a discord api endpoint
  /// @param endpoint endpoint to send the request to
  /// @param data postdata to be sent
  std::expected<std::string, RestErrors> post(const std::string& endpoint,
                                              const std::string& data);

  /// @brief send a patch request to a discord api endpoint
  /// @param endpoint endpoint to send the request to
  /// @param data postdata to be sent
  std::expected<std::string, RestErrors> patch(const std::string& endpoint,
                                               const std::string& data);

  /// @brief send a put request to a discord api endpoint
  /// @param endpoint endpoint to send the request to
  /// @param data postdata to be sent
  std::expected<std::string, RestErrors> put(const std::string& endpoint,
                                             const std::string& data);

  /// @brief send form data  to a discord api endpoint
  /// @param endpoint endpoint to send the request to
  /// @param data postdata to be sent
  /// @param method method to use
  std::expected<std::string, RestErrors> sendFormData(const std::string& endpoint,
                                                      const std::string& data,
                                                      const std::string& boundary,
                                                      const std::string& method);
private:
  std::expected<std::string, RestErrors> sendRawData(const std::string& endpoint,
                                                     const std::string& method,
                                                     const std::vector<std::string>& headers,
                                                     const std::optional<std::string>& data = std::nullopt);
  NyaBot *bot;
  RateLimitTables rtl;
};

#endif
