#include "../include/base64.h"
#include <string>
#include <string_view>
#include <cstdint>
#include <array>



constexpr std::array<uint8_t, 64> base64_map = {
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
  'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
  'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
  'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
  'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
  'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
  'w', 'x', 'y', 'z', '0', '1', '2', '3',
  '4', '5', '6', '7', '8', '9', '+', '/'
};


std::string encodeB64(const std::string_view a){
  std::basic_string_view<uint8_t> str(reinterpret_cast<uint8_t const*>(a.data()), a.size());
  std::string encoded;
  encoded.reserve(a.length());
  while(str.length() > 0){
    if(str.length() >= 3){
      const uint32_t val = (str.at(0) << 16) | (str.at(1) << 8) | str.at(2);
      for(int i = 3; i >= 0; --i){
        const uint8_t chr = (val & (0b00111111 << i * 6)) >> i * 6;
        encoded += base64_map.at(chr);
      }
      str = str.substr(3);
    }
    else if(str.length() == 2){
      const uint32_t val = (str.at(0) << 16) | str.at(1) << 8;
      for(int i = 3; i > 0; --i){
        const uint8_t chr = (val & (0b00111111 << i * 6)) >> i * 6;
        encoded += base64_map.at(chr);
      }
      encoded += '=';
      break;
    }
    else if(str.length() == 1){
      const uint8_t val = str.at(0);
      const uint8_t chr = (val & 0b11111100) >> 2;
      encoded += base64_map.at(chr);
      const uint8_t chr2 = (val & 0b00000011) << 4;
      encoded += base64_map.at(chr2);
      encoded += "==";
      break;
    }
  }
  return encoded;
}
