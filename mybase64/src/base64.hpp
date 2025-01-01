#include <array>
#include <iostream>
#include <span>
#include <cassert>
#include <string>

namespace base64
{
  std::array<char,
             64> constexpr encode_table{'A', 'B', 'C', 'D', 'E', 'F', 'G',
                                        'H', 'I', 'J', 'K', 'L', 'M', 'N',
                                        'O', 'P', 'Q', 'R', 'S', 'T', 'U',
                                        'V', 'W', 'X', 'Y', 'Z', 'a', 'b',
                                        'c', 'd', 'e', 'f', 'g', 'h', 'i',
                                        'j', 'k', 'l', 'm', 'n', 'o', 'p',
                                        'q', 'r', 's', 't', 'u', 'v', 'w',
                                        'x', 'y', 'z', '0', '1', '2', '3',
                                        '4', '5', '6', '7', '8', '9', '+',
                                        '/'}; // base64 [0,63] 每个字符使用6bit

  std::array<char,4> encode_triplet(std::span<const std::uint8_t> input)
  {
    assert(input.size() == 3);
    std::array<char,4> result;
    result[0] = encode_table[(input[0] & 0b11111100) >> 2];
    result[1] = encode_table[((input[0] & 0b00000011) << 4) | ((input[1] & 0b11110000) >> 4)];
    result[2] = encode_table[((input[1] & 0b00001111) << 2) | ((input[2] & 0b11000000) >> 6)];
    result[3] = encode_table[input[2] & 0b00111111];
    return result;
  }

  std::string encode(std::span<std::uint8_t const> input){
    std::string output;
    int i = 0;
    while(i+3 <= input.size()){
      auto triplet = encode_triplet(input.subspan(i, 3));
      output.append(triplet.data(), triplet.size());
      i += 3;
    } // 成组的直接交给encode_triplet处理

    int remaining_chars = input.size() - i;
    
    if(remaining_chars == 2){
      std::array<std::uint8_t, 3> temp{input[i], input[i+1], 0};
      auto triplet = encode_triplet(temp);
      output.append(triplet.data(), 3);
      output += '=';
    }
    else if(remaining_chars == 1){
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

} // namespace base64
