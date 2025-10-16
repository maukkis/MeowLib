#ifndef _INCLUDE_POLL_H
#define _INCLUDE_POLL_H
#include <nlohmann/json_fwd.hpp>
#include <string_view>
#include <type_traits>
#include "emoji.h"

enum class PollLayoutTypes {
  DEFAULT = 1,
};

struct MessagePollVote {
  MessagePollVote(const nlohmann::json& j);
  std::string userId;
  std::string channelId;
  std::string messageId;
  std::optional<std::string> guildId;
  int answerId{};
};

struct PollMedia {
  PollMedia() = default;
  PollMedia(const nlohmann::json& j);
  nlohmann::json generate() const;
  std::optional<std::string> text;
  std::optional<Emoji> emoji;
};

struct PollAnswer {
  PollAnswer() = default;
  PollAnswer(const nlohmann::json& j);
  nlohmann::json generate() const;
  PollAnswer& setText(const std::string_view);
  int answerId{};
  PollMedia pollMedia;
};

struct PollAnswerCount {
  PollAnswerCount() = default;
  PollAnswerCount(const nlohmann::json& j);
  int id{};
  int count{};
  bool meVoted{};
};


struct PollResults {
  PollResults() = default;
  PollResults(const nlohmann::json& j);
  bool isFinalized{};
  std::vector<PollAnswerCount> answerCounts;
};

struct Poll {
  Poll() = default;
  Poll(const nlohmann::json& j);
  nlohmann::json generate() const;
  Poll& setQuestion(const std::string_view a);
  template<typename... T>
  requires(std::conjunction<std::is_same<std::remove_reference_t<T>, PollAnswer>...>::value)
  Poll& setAnswers(T&&... a){
    static_assert(sizeof...(a) <= 10, "cannot have more than 10 questions");
    (answers.emplace_back(std::forward<T>(a)), ...);
    return *this;
  }
  Poll& setDuration(int duration);
  Poll& allowMultiselect(bool woof);
  PollMedia question;
  std::vector<PollAnswer> answers;
  bool allowMultiselectv = false;
  int duration = 24;
  // "expiry is marked as nullable to support non-expiring polls in the future,
  // but all polls have an expiry currently." -- discord api docs
  std::optional<std::string> expiry;
  PollLayoutTypes layoutType = PollLayoutTypes::DEFAULT;
  std::optional<PollResults> results;
};


#endif
