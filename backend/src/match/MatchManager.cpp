#include "match/MatchManager.h"

#include "room/RoomManager.h"

namespace animalfight {

MatchManager::MatchManager(RoomManager &rooms) : rooms_(rooms) {}

MatchStatus MatchManager::join(int userId) {
    std::lock_guard lock(mutex_);
    if (const auto it = status_.find(userId); it != status_.end() && (it->second.searching || it->second.matched)) return it->second;
    if (!waitingUser_ || *waitingUser_ == userId) {
        waitingUser_ = userId;
        return status_[userId] = {true, false, ""};
    }
    const auto opponentId = *waitingUser_;
    waitingUser_.reset();
    const auto roomId = rooms_.create(opponentId, false);
    rooms_.join(roomId, userId);
    status_[opponentId] = {false, true, roomId};
    return status_[userId] = {false, true, roomId};
}

MatchStatus MatchManager::status(int userId) const {
    std::lock_guard lock(mutex_);
    const auto it = status_.find(userId);
    return it == status_.end() ? MatchStatus{} : it->second;
}

void MatchManager::cancel(int userId) {
    std::lock_guard lock(mutex_);
    if (waitingUser_ && *waitingUser_ == userId) waitingUser_.reset();
    status_.erase(userId);
}

}  // namespace animalfight
