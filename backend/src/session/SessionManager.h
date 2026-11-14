#pragma once

#include <chrono>
#include <optional>
#include <shared_mutex>
#include <string>
#include <unordered_map>

namespace animalfight {

struct Session {
    int userId{};
    std::string username;
    std::chrono::system_clock::time_point expiresAt;
};

class SessionManager {
  public:
    std::string create(int userId, std::string username);
    std::optional<Session> find(const std::string &sessionId) const;
    void revoke(const std::string &sessionId);
    void cleanupExpired();

  private:
    mutable std::shared_mutex mutex_;
    std::unordered_map<std::string, Session> sessions_;
};

}  // namespace animalfight

