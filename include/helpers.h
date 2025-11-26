#ifndef _INCLUDE_HELPERS_H
#define _INCLUDE_HELPERS_H
#include <chrono>
#include <format>
#include <sstream>
#include "componentsv2/actionRowComponent.h"
#include <string>
#include <nlohmann/json.hpp>
#include <vector>
#include "attachment.h"

template<typename T>
std::string timepointToISO8601(const std::chrono::time_point<T>& time){
  return std::format("{:%FT%TZ}", time);
}


struct SelectComponent;
class LabelComponent;
class ButtonComponent;
class TextInputComponent;
class FileUploadComponent;

template<typename T, typename = void>
struct TopLevelComponent : std::true_type {};

template<>
struct TopLevelComponent<ActionRowComponent<NONE>> : std::false_type {};

template<typename T>
struct TopLevelComponent<T, std::enable_if_t<std::is_base_of_v<SelectComponent, T>, void>> : std::false_type {};

template<>
struct TopLevelComponent<ButtonComponent> : std::false_type {};

template<>
struct TopLevelComponent<LabelComponent> : std::false_type {};

template<>
struct TopLevelComponent<TextInputComponent> : std::false_type {};

template<>
struct TopLevelComponent<FileUploadComponent> : std::false_type {};

template<typename T>
concept topLevelComponent = TopLevelComponent<std::remove_cvref_t<T>>::value;


auto ISO8601ToTimepoint(const std::string& str);

std::string makeFormData(const nlohmann::json j, const std::string_view boundary, const std::vector<Attachment>& a);
std::string& lower(std::string& a);
std::string attachmentToDataUri(const Attachment& a);
std::string asciiToURLEncoded(const std::string_view a);
std::string generate32ByteASCIINonce();
#endif
