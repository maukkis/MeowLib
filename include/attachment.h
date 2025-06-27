#ifndef _INCLUDE_ATTACHMENT_H
#define _INCLUDE_ATTACHMENT_H
#include <string>


struct Attachment {
  Attachment(const std::string& file);
  Attachment() = default;
  std::string data;
  std::string name;
};


Attachment readFile(const std::string& a);
#endif
