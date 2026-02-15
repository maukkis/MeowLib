#ifndef _INCLUDE_MODALINTERACTION_H
#define _INCLUDE_MODALINTERACTION_H
#include "attachment.h"
#include "componentsv2/componentsv2.h"
#include "interaction.h"
#include <string>
#include <variant>
#include <unordered_set>


using SelectData = std::unordered_set<std::string>;
using TextInputData = std::string;
using FileUploadData = std::unordered_set<std::string>;
using RadioActionGroupData = std::string;
using CheckboxData = bool;
using CheckboxGroupData = std::unordered_set<std::string>;

struct ModalData {
  template<typename T>
  T get(){
    return std::get<T>(data);
  }
  ComponentTypes type;
  // std::string if Text Input std::unordered_set<std::string> if select or file upload
  std::variant<std::string, std::unordered_set<std::string>, CheckboxData> data;
};

class ModalInteraction : public Interaction {
public:
  using Interaction::Interaction;
  std::unordered_map<std::string, ModalData> data;
  std::unordered_map<std::string, User> resolvedUsers;
  std::unordered_map<std::string, ResolvedAttachment> resolvedAttachments;
};


#endif
