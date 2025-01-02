#include <array>
#include <iostream>
#include <span>
#include <cassert>
#include <string>
#include <vector>

namespace base64
{
  std::array<char, 64> constexpr encode_table{
      'A', 'B', 'C', 'D', 'E', 'F', 'G',
      'H', 'I', 'J', 'K', 'L', 'M', 'N',
      'O', 'P', 'Q', 'R', 'S', 'T', 'U',
      'V', 'W', 'X', 'Y', 'Z', 'a', 'b',
      'c', 'd', 'e', 'f', 'g', 'h', 'i',
      'j', 'k', 'l', 'm', 'n', 'o', 'p',
      'q', 'r', 's', 't', 'u', 'v', 'w',
      'x', 'y', 'z', '0', '1', '2', '3',
      '4', '5', '6', '7', '8', '9', '+',
      '/'}; // base64 [0,63] 每个字符使用6bit

  std::array<std::uint8_t, 128> constexpr decode_table{
      0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64,
      0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64,
      0x64, 0x64, 0x64, 0x64, 0x64, 0x3E, 0x64, 0x64, 0x64, 0x3F, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C,
      0x3D, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A,
      0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x64, 0x64, 0x64, 0x64,
      0x64, 0x64, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A,
      0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x64, 0x64, 0x64, 0x64, 0x64};
      // ASCII表 非base64字符用0x64表示，即非法字符 decode_table['A] = decode_table[65] = 0x00

  std::array<char, 4> encode_triplet(std::span<const std::uint8_t> input)
  {
    assert(input.size() == 3);
    
    auto base64_char0 = encode_table[(input[0] & 0b11111100) >> 2];
    auto base64_char1 = encode_table[((input[0] & 0b00000011) << 4) | ((input[1] & 0b11110000) >> 4)];
    auto base64_char2 = encode_table[((input[1] & 0b00001111) << 2) | ((input[2] & 0b11000000) >> 6)];
    auto base64_char3 = encode_table[input[2] & 0b00111111];
    return {base64_char0, base64_char1, base64_char2, base64_char3};
  }

  std::string encode(std::span<std::uint8_t const> input)
  {
    std::string output;
    int i = 0;
    while (i + 3 <= input.size())
    {
      auto triplet = encode_triplet(input.subspan(i, 3));
      output.append(triplet.data(), triplet.size());
      i += 3;
    } // 成组的直接交给encode_triplet处理

    int remaining_chars = input.size() - i;

    if (remaining_chars == 2)
    {
      std::array<std::uint8_t, 3> temp{input[i], input[i + 1], 0};
      auto triplet = encode_triplet(temp);
      output.append(triplet.data(), 3);
      output += '=';
    }
    else if (remaining_chars == 1)
    {
      std::array<std::uint8_t, 3> temp{input[i], 0, 0};
      auto triplet = encode_triplet(temp);
      output.append(triplet.data(), 2);
      output += "==";
    }

    return output;
  }

  std::string encode_str(std::string_view input)
  {
    return encode({reinterpret_cast<std::uint8_t const *>(input.data()), input.size()});
  }

  std::array<std::uint8_t, 3> decode_quad(std::span<const char> input){
    std::uint32_t const concat_bytes = 
      (decode_table[static_cast<std::uint8_t>(input[0])] << 18) |
      (decode_table[static_cast<std::uint8_t>(input[1])] << 12) |
      (decode_table[static_cast<std::uint8_t>(input[2])] << 6) |
      (decode_table[static_cast<std::uint8_t>(input[3])]);

    std::uint8_t const byte0 = (concat_bytes & 0xFF0000) >> 16;
    std::uint8_t const byte1 = (concat_bytes & 0x00FF00) >> 8;
    std::uint8_t const byte2 = (concat_bytes & 0x0000FF);
    return {byte0, byte1, byte2};
  }

  std::vector<uint8_t> decode(std::string_view input){
    std::vector<uint8_t> output;
    int i = 0;
    while(i + 4 <= input.size()){
      auto quad = decode_quad(input.substr(i, 4));
      output.insert(output.end(), quad.begin(), quad.end());
      i += 4;
    }

    int remaining_chars = input.size() - i;
    if(remaining_chars == 3){
      std::array<char, 4> temp{input[i], input[i+1], input[i+2], 0};
      auto quad = decode_quad(temp);
      output.insert(output.end(), quad.begin(), quad.end() - 1);
    }else if(remaining_chars == 2){
      std::array<char, 4> temp{input[i], input[i+1], 0, 0};
      auto quad = decode_quad(temp);
      output.insert(output.end(), quad.begin(), quad.end() - 2);
    }else if(remaining_chars == 1){
      throw std::invalid_argument("Invalid base64 input");
    }

    return output;
  }

} // namespace base64
