#include <array>
#include <iostream>

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

  std::array<char, 4> encode_triplet(std::uint8_t a, std::uint8_t b,
                                     std::uint8_t c)
  {
    std::array<char, 4> result;
    result[0] = encode_table[(a & 0xfc) >> 2];
    result[1] = encode_table[((a & 0x03) << 4) | ((b & 0xf0) >> 4)];
    result[2] = encode_table[((b & 0x0f) << 2) | ((c & 0xc0) >> 6)];
    result[3] = encode_table[c & 0x3f];
    return result;
  }

  template <typename Container>
  std::array<char, 4> encode_triplet(const Container &container)
    requires(std::ranges::range<Container> &&
             std::is_same_v<std::ranges::range_value_t<Container>, std::uint8_t>)
  {
    assert(std::ranges::size(container) == 3);

    auto it = std::ranges::begin(container);
    return encode_triplet(*it, *(it + 1), *(it + 2));
  }

  template<typename T, std::size_t N>
  std::array<char,4> encode_triplet(const T (&array)[N])
    requires(std::is_same_v<T, std::uint8_t>)
  {
    static_assert(N == 3);
    return encode_triplet(array[0], array[1], array[2]);
  }

} // namespace base64
