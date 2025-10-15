#ifndef _INCLUDE_COMPONENTSV2_FILEUPLOAD_H
#define _INCLUDE_COMPONENTSV2_FILEUPLOAD_H
#include "componentsv2.h"


class FileUploadComponent : public Component {
public:
  nlohmann::json generate() override;
  FileUploadComponent& setCustomId(const std::string_view id);
  FileUploadComponent& setMinValues(int values);
  FileUploadComponent& setMaxValues(int values);
  FileUploadComponent& setRequired(bool required);
private:
  ComponentTypes type = FILE_UPLOAD;
  std::string customId;
  int minValues = 1;
  int maxValues = 1;
  bool required = true;
};


#endif
