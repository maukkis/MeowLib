#ifndef _INCLUDE_MODALINTERACTION_H
#define _INCLUDE_MODALINTERACTION_H
#include "componentsv2/componentsv2.h"
#include "interaction.h"
#include <string>
#include <unordered_set>


/// @deprecated use SelectData instead
using StringSelectData = std::unordered_set<std::string>;
using SelectData = std::unordered_set<std::string>;
using TextInputData = std::string;

struct ModalData {
  template<typename T>
  T get(){
    return std::get<T>(data);
  }
  ComponentTypes type;
  // std::string if Text Input std::unordered_set<std::string> if select
  std::variant<std::string, std::unordered_set<std::string>> data;
};

class ModalInteraction : public Interaction {
public:
  using Interaction::Interaction;
  std::unordered_map<std::string, ModalData> data;
  std::unordered_map<std::string, User> resolvedUsers;
};


#endif
