#include "../include/base64.h"
#include <string>
#include <string_view>
#include <cstdint>
#include <unordered_map>


const std::unordered_map<uint8_t, char> base64_map = {
  {0, 'A'}, {1, 'B'}, {2, 'C'}, {3, 'D'}, {4, 'E'}, {5, 'F'},
  {6, 'G'}, {7, 'H'}, {8, 'I'}, {9, 'J'}, {10, 'K'}, {11, 'L'},
  {12, 'M'}, {13, 'N'}, {14, 'O'}, {15, 'P'}, {16, 'Q'}, {17, 'R'},
  {18, 'S'}, {19, 'T'}, {20, 'U'}, {21, 'V'}, {22, 'W'}, {23, 'X'},
  {24, 'Y'}, {25, 'Z'}, {26, 'a'}, {27, 'b'}, {28, 'c'}, {29, 'd'},
  {30, 'e'}, {31, 'f'}, {32, 'g'}, {33, 'h'}, {34, 'i'}, {35, 'j'},
  {36, 'k'}, {37, 'l'}, {38, 'm'}, {39, 'n'}, {40, 'o'}, {41, 'p'},
  {42, 'q'}, {43, 'r'}, {44, 's'}, {45, 't'}, {46, 'u'}, {47, 'v'},
  {48, 'w'}, {49, 'x'}, {50, 'y'}, {51, 'z'}, {52, '0'}, {53, '1'},
  {54, '2'}, {55, '3'}, {56, '4'}, {57, '5'}, {58, '6'}, {59, '7'},
  {60, '8'}, {61, '9'}, {62, '+'}, {63, '/'}
};

std::string encodeB64(const std::string_view a){
  std::string_view str = a;
  std::string encoded;
  encoded.reserve(a.length());
  while(str.length() > 0){
    if(str.length() >= 3){
      const uint32_t val = (static_cast<uint8_t>(str[0]) << 16) | (static_cast<uint8_t>(str[1]) << 8) | static_cast<uint8_t>(str[2]);
      for(ssize_t i = 3; i >= 0; --i){
        const uint8_t chr = (val & (0b00111111 << i * 6)) >> i * 6;
        encoded += base64_map.at(chr);
      }
      str = str.substr(3);
    }
    else if(str.length() == 2){
      const uint32_t val = (static_cast<uint8_t>(str[0]) << 16) | static_cast<uint8_t>(str[1]) << 8;
      for(ssize_t i = 3; i > 0; --i){
        const uint8_t chr = (val & (0b00111111 << i * 6)) >> i * 6;
        encoded += base64_map.at(chr);
      }
      encoded += '=';
      break;
    }
    else if(str.length() == 1){
      const uint8_t val = static_cast<uint8_t>(str[0]);
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
