#ifndef _INCLUDE_HELPERS_H
#define _INCLUDE_HELPERS_H
#include <string>
#include <nlohmann/json.hpp>
#include <vector>
#include "attachment.h"

std::string makeFormData(const nlohmann::json j, const std::string_view boundary, const std::vector<Attachment>& a);

#endif
