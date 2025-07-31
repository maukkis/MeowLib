#ifndef _INCLUDE_MESSAGE_H
#define _INCLUDE_MESSAGE_H
#include "componentsv2/componentsv2.h"
#include "attachment.h"
#include <nlohmann/json.hpp>
#include <type_traits>
#include <vector>
#include <memory>


class Message {
public:
  nlohmann::json generate() const;
  Message() = default;
  Message(Message&&) = default;
  template<typename T>
  requires(std::is_base_of_v<Component, std::remove_reference_t<T>>)
  Message& addComponent(T&& comp){
    components.emplace_back(std::make_unique<std::remove_cvref_t<T>>(std::forward<T>(comp)));
    return *this;
  }
  Message& addAttachment(const Attachment& a);
  Message& setContent(const std::string_view a);
  std::vector<Attachment> attachments;
private:
  std::string content;
  std::vector<std::unique_ptr<Component>> components;
};

#endif

