#ifndef _INCLUDE_EMOJI_H
#define _INCLUDE_EMOJI_H
#include <expected>
#include <string>
#include <optional>
#include <nlohmann/json.hpp>
#include "attachment.h"
#include "user.h"
#include <string_view>
class NyaBot;


struct Emoji {
  std::string id;
  std::string name;
  std::optional<bool> animated = std::nullopt;
  std::optional<User> user = std::nullopt;
};

class EmojiApiRoutes {
public:
  EmojiApiRoutes(NyaBot *bot);
  /// @brief creates an application emoji
  /// @param file to be uploaded
  /// @param name name of the emoji
  /// @returns Emoji object on sucess nothing on failure 
  std::expected<Emoji, std::nullopt_t> createApplicationEmoji(const std::string_view name, const Attachment& file);
  /// @brief modifies an application emoji
  /// @param id emoji id to edit
  /// @param name new name for the emoji
  /// @returns new emoji object on success nothing on failure
  std::expected<Emoji, std::nullopt_t> modifyApplicationEmoji(const std::string_view id, const std::string_view name);
  /// @brief deletes an application emoji
  /// @param id emoji id to delete
  void deleteApplicationEmoji(const std::string_view id);
  /// @brief gets an application emoji object
  /// @param id emoji id to get
  /// @returns Emoji object on success nothing on error
  std::expected<Emoji, std::nullopt_t> getApplicationEmoji(const std::string_view id);
  /// @brief lists all application emojis
  /// @returns map of id's to emojis
  std::expected<std::unordered_map<std::string, Emoji>, std::nullopt_t> listApplicationEmojis();
private:
  const std::string_view appId;
  NyaBot *bot;
};

Emoji deserializeEmoji(const nlohmann::json& j);
nlohmann::json serializeEmoji(const Emoji& e);

#endif
