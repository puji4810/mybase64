#include "base64.hpp"
#include <random>

std::string generate_random_string() {
  std::string random_string;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, 63);
  int n = dis(gen);
  for (; n < 100; ++n) {
    random_string.push_back(base64::encode_table[(dis(gen))]);
  }
  return random_string;
}

void test_base64() {
  std::string input = generate_random_string();
  // std::cout << "Input: " << input << std::endl;
  std::string output = base64::encode_str(input);
  // std::cout << "Output: " << output << std::endl;
  std::string decoded = base64::decode_str(output);
  // std::cout << "Decoded: " << decoded << std::endl;
  assert(input == decoded);
}

int main() {
  for (int i = 0; i < 10000; ++i) {
    test_base64();
  }
  return 0;
}
