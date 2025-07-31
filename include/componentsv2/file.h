#ifndef _INCLUDE_COMPONENTSV2_FILE_H
#define _INCLUDE_COMPONENTSV2_FILE_H
#include "componentsv2.h"
#include <string>
#include <string_view>

class FileComponent : public Component {
public:
  nlohmann::json generate() override;
  // NOTE: file must be added to the message and added to this component like this attachment://filename.extension
  FileComponent& setFile(const std::string_view a);
  FileComponent& setSpoiler(bool spoiler);
private:
  ComponentTypes type = ComponentTypes::FILE_TYPE;
  std::string fileName;
  bool spoiler = false;
};


#endif
