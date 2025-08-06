#ifndef _INCLUDE_ATTACHMENT_H
#define _INCLUDE_ATTACHMENT_H
#include <string>


struct Attachment {
  std::string data;
  std::string name;
};

/// @brief Creates an Attachment object from a file
/// @param a path to a file
///
Attachment readFile(const std::string& a);
#endif
