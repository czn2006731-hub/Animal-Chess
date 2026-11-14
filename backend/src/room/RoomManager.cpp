#include "room/RoomManager.h"

#include "utils/Utils.h"

namespace animalfight {

std::string RoomManager::createIdLocked() {
    std::string id;
    do { id = Utils::randomToken(4).substr(0, 8); } while (rooms_.count(id));
    return id;
}

std::string RoomManager::create(int hostId, bool isPrivate) {
    std::lock_guard lock(mutex_);
    const auto id = createIdLocked();
    rooms_.emplace(id, RoomInfo{id, hostId, std::nullopt, isPrivate});
    return id;
}

std::optional<RoomInfo> RoomManager::join(const std::string &roomId, int userId) {
    std::lock_guard lock(mutex_);
    const auto it = rooms_.find(roomId);
    if (it == rooms_.end() || it->second.guestId || it->second.hostId == userId) return std::nullopt;
    it->second.guestId = userId;
    return it->second;
}

std::optional<RoomInfo> RoomManager::find(const std::string &roomId) const {
    std::lock_guard lock(mutex_);
    const auto it = rooms_.find(roomId);
    return it == rooms_.end() ? std::nullopt : std::optional<RoomInfo>{it->second};
}

}  // namespace animalfight
