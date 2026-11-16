#pragma once

#include <memory>
#include <mutex>

namespace animalfight {

class Database;
class SessionManager;
class UserManager;
class MatchManager;
class RoomManager;

class ServerApp {
  public:
    void initialize();

    static Database &database();
    static SessionManager &sessions();
    static UserManager &users();
    static MatchManager &matches();
    static RoomManager &rooms();

  private:
    static std::unique_ptr<Database> database_;
    static std::mutex databaseMutex_;
    static std::unique_ptr<SessionManager> sessions_;
    static std::unique_ptr<UserManager> users_;
    static std::unique_ptr<MatchManager> matches_;
    static std::unique_ptr<RoomManager> rooms_;
};

}  // namespace animalfight

