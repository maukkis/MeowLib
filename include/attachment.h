#ifndef _INCLUDE_ATTACHMENT_H
#define _INCLUDE_ATTACHMENT_H
#include <string>
#include <nlohmann/json_fwd.hpp>
#include <optional>

struct ResolvedAttachment {
  ResolvedAttachment() = default;
  ResolvedAttachment(const nlohmann::json& j);
  std::string filename;
  std::string id;
  std::optional<std::string> title = std::nullopt;
  std::optional<std::string> description = std::nullopt;
  std::optional<std::string> contentType = std::nullopt;
  int size{};
  std::string url;
  std::string proxyUrl;
  std::optional<int> height = std::nullopt;
  std::optional<int> width = std::nullopt;
  std::optional<bool> ephemeral = std::nullopt;
};


struct Attachment {
  std::string data;
  std::string name;
};

/// @brief Creates an Attachment object from a file
/// @param a path to a file
///
Attachment readFile(const std::string& a);
#endif
