#pragma once

#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>

namespace animalfight {

struct RoomInfo {
    std::string id;
    int hostId{};
    std::optional<int> guestId;
    bool isPrivate{};
};

class RoomManager {
  public:
    std::string create(int hostId, bool isPrivate);
    std::optional<RoomInfo> join(const std::string &roomId, int userId);
    std::optional<RoomInfo> find(const std::string &roomId) const;

  private:
    std::string createIdLocked();
    mutable std::mutex mutex_;
    std::unordered_map<std::string, RoomInfo> rooms_;
};

}  // namespace animalfight
