#pragma once

#include <string>

namespace animalfight {

class Utils {
  public:
    static std::string randomToken(std::size_t byteCount);
    static bool isValidUsername(const std::string &username);
    static int levelForScore(int score);
    static std::string sha256(const std::string &value);
};

}  // namespace animalfight

