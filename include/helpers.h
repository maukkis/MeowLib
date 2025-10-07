#ifndef _INCLUDE_HELPERS_H
#define _INCLUDE_HELPERS_H
#include <chrono>
#include <format>
#include <string>
#include <nlohmann/json.hpp>
#include <vector>
#include "attachment.h"

template<typename T>
std::string timepointToISO8601(const std::chrono::time_point<T>& time){
  return std::format("{:%FT%TZ}", time);
}

std::string makeFormData(const nlohmann::json j, const std::string_view boundary, const std::vector<Attachment>& a);
std::string& lower(std::string& a);
std::string attachmentToDataUri(const Attachment& a);
std::string generate32ByteASCIINonce();
#endif
