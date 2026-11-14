#include "session/SessionManager.h"

#include <mutex>

#include "utils/Utils.h"

namespace animalfight {

std::string SessionManager::create(int userId, std::string username) {
    const auto id = Utils::randomToken(48);
    const auto expires = std::chrono::system_clock::now() + std::chrono::hours(24 * 7);
    std::unique_lock lock(mutex_);
    sessions_[id] = Session{userId, std::move(username), expires};
    return id;
}

std::optional<Session> SessionManager::find(const std::string &sessionId) const {
    std::shared_lock lock(mutex_);
    const auto it = sessions_.find(sessionId);
    if (it == sessions_.end() || it->second.expiresAt <= std::chrono::system_clock::now()) return std::nullopt;
    return it->second;
}

void SessionManager::revoke(const std::string &sessionId) {
    std::unique_lock lock(mutex_);
    sessions_.erase(sessionId);
}

void SessionManager::cleanupExpired() {
    const auto now = std::chrono::system_clock::now();
    std::unique_lock lock(mutex_);
    for (auto it = sessions_.begin(); it != sessions_.end();) {
        if (it->second.expiresAt <= now) it = sessions_.erase(it); else ++it;
    }
}

}  // namespace animalfight

