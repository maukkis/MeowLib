#include "../include/poll.h"
#include <nlohmann/json.hpp>


MessagePollVote::MessagePollVote(const nlohmann::json& j){
  userId = j["user_id"];
  channelId = j["channel_id"];
  messageId = j["message_id"];
  if(j.contains("guild_id"))
    guildId = j["guild_id"];
  answerId = j["answer_id"];
}



PollMedia::PollMedia(const nlohmann::json& j){
  if(j.contains("text"))
    text = j["text"];
  if(j.contains("emoji"))
    emoji = deserializeEmoji(j["emoji"]);
}


PollAnswer::PollAnswer(const nlohmann::json& j){
  if(j.contains("answer_id"))
    answerId = j["answer_id"];
  pollMedia = PollMedia(j["poll_media"]);
}

PollAnswerCount::PollAnswerCount(const nlohmann::json& j){
  id = j["id"];
  count = j["count"];
  meVoted = j["me_voted"];
}


PollResults::PollResults(const nlohmann::json& j){
  isFinalized = j["is_finalized"];
  for(const auto& a : j["answer_counts"])
    answerCounts.emplace_back(a);
}

Poll::Poll(const nlohmann::json& j){
  question = PollMedia(j["question"]);
  for(const auto& a : j["answers"]){
    answers.emplace_back(a);
  }
  if(!j["expiry"].is_null())
    expiry = j["expiry"];
  allowMultiselectv = j["allow_multiselect"];
  layoutType = j["layout_type"];
  if(j.contains("results"))
    results = PollResults(j["results"]);
}

Poll& Poll::setDuration(int duration){
  this->duration = duration;
  return *this;
}


Poll& Poll::allowMultiselect(bool woof){
  allowMultiselectv = woof;
  return *this;
}

Poll& Poll::setQuestion(const std::string_view a){
  question.text = a;
  return *this;
}


PollAnswer& PollAnswer::setText(const std::string_view a){
  pollMedia.text = a;
  return *this;
}


nlohmann::json PollMedia::generate() const {
  nlohmann::json j;
  j["text"] = text;
  return j;
}


nlohmann::json PollAnswer::generate() const {
  nlohmann::json j;
  j["poll_media"] = pollMedia.generate();
  return j;
}


nlohmann::json Poll::generate() const {
  nlohmann::json j;
  j["question"] = question.generate();
  for(const auto& a : answers){
    j["answers"].emplace_back(a.generate());
  }
  j["duration"] = duration;
  j["allow_multiselect"] = allowMultiselectv;
  j["layout_type"] = layoutType;
  return j;
}
