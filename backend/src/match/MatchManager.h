#pragma once

#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>

namespace animalfight {

class RoomManager;
struct MatchStatus { bool searching{}; bool matched{}; std::string roomId; };

class MatchManager {
  public:
    explicit MatchManager(RoomManager &rooms);
    MatchStatus join(int userId);
    MatchStatus status(int userId) const;
    void cancel(int userId);

  private:
    RoomManager &rooms_;
    mutable std::mutex mutex_;
    std::optional<int> waitingUser_;
    std::unordered_map<int, MatchStatus> status_;
};

}  // namespace animalfight
