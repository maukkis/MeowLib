#ifndef _INCLUDE_RESTCLIENT_H
#define _INCLUDE_RESTCLIENT_H
#include <atomic>
#include <string>
#include <mutex>
#include <optional>
#include <unordered_map>
#include <vector>
#include <expected>
#include <string_view>



class NyaBot;

constexpr std::string_view UserAgent {R"(DiscordBot (https://git.girlsmell.xyz/luna/MeowLib, 0.1.0))"};

// TODO: add rest of the errors here
enum class RestErrors {
  IOERR
};

struct RateLimit {
  std::string bucket;
  int resetAfter;
  int reset;
};

struct RateLimitTables {
  std::unordered_map<std::string, RateLimit> userRateLimitTable;
  std::mutex rltmtx;
  std::atomic<bool> globalLimit;
  std::atomic<int> globalResetAfter;
};

class RestClient {
public:
  RestClient(NyaBot *bot);
  /// @brief send a get request to a discord api endpoint
  /// @param endpoint endpoint to send the request to
  std::expected<std::pair<std::string, int>, RestErrors> get(const std::string& endpoint);

  /// @brief send a post request to a discord api endpoint
  /// @param endpoint endpoint to send the request to
  /// @param data postdata to be sent
  std::expected<std::pair<std::string, int>, RestErrors> post(const std::string& endpoint,
                                              const std::string& data);

  /// @brief send a patch request to a discord api endpoint
  /// @param endpoint endpoint to send the request to
  /// @param data postdata to be sent
  std::expected<std::pair<std::string, int>, RestErrors> patch(const std::string& endpoint,
                                               const std::string& data);

  /// @brief send a put request to a discord api endpoint
  /// @param endpoint endpoint to send the request to
  /// @param data postdata to be sent
  std::expected<std::pair<std::string, int>, RestErrors> put(const std::string& endpoint,
                                             const std::string& data);

  /// @brief send a delete request to a discord api endpoint
  /// @param endpoint endpoint to send the request to
  // stupid C++ having delete as a keyword
  std::expected<std::pair<std::string, int>, RestErrors> deletereq(const std::string& endpoint);
  /// @brief send form data  to a discord api endpoint
  /// @param endpoint endpoint to send the request to
  /// @param data postdata to be sent
  /// @param method method to use
  std::expected<std::pair<std::string, int>, RestErrors> sendFormData(const std::string& endpoint,
                                                      const std::string& data,
                                                      const std::string& boundary,
                                                      const std::string& method);
private:
  std::expected<std::pair<std::string, int>, RestErrors> sendRawData(const std::string& endpoint,
                                                     const std::string& method,
                                                     const std::vector<std::string>& headers,
                                                     const std::optional<std::string>& data = std::nullopt);
  NyaBot *bot;
  RateLimitTables rtl;
};

#endif
