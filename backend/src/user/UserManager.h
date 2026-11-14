#pragma once

#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace animalfight {

struct UserProfile {
    int id{};
    std::string username;
    int score{};
    int level{1};
    int wins{};
    int losses{};
    int draws{};
};

struct RankItem : UserProfile {
    int rank{};
    int winRate{};
};

class UserManager {
  public:
    using UserCallback = std::function<void(std::optional<UserProfile>, const std::string &)>;
    using RankingCallback = std::function<void(std::vector<RankItem>, const std::string &)>;

    void registerUser(const std::string &username, const std::string &password, UserCallback callback) const;
    void login(const std::string &username, const std::string &password, UserCallback callback) const;
    void findById(int userId, UserCallback callback) const;
    void ranking(int limit, RankingCallback callback) const;
};

}  // namespace animalfight
