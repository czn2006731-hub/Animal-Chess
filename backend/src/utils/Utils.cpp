#include "utils/Utils.h"

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <random>
#include <sstream>
#include <openssl/sha.h>

namespace animalfight {

std::string Utils::randomToken(std::size_t byteCount) {
    std::random_device device;
    std::mt19937_64 engine(device());
    std::uniform_int_distribution<unsigned int> distribution(0, 255);
    std::ostringstream result;
    for (std::size_t i = 0; i < byteCount; ++i) result << std::hex << std::setw(2) << std::setfill('0') << distribution(engine);
    return result.str();
}

bool Utils::isValidUsername(const std::string &username) {
    return username.size() >= 3 && username.size() <= 20 &&
           std::all_of(username.begin(), username.end(), [](unsigned char c) { return std::isalnum(c) || c == '_'; });
}

int Utils::levelForScore(int score) { return std::min(100, std::max(1, score / 10 + 1)); }

std::string Utils::sha256(const std::string &value) {
    unsigned char digest[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char *>(value.data()), value.size(), digest);
    std::ostringstream result;
    for (const auto byte : digest) result << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    return result.str();
}

}  // namespace animalfight

