#include "ServerApp.h"

#include <stdexcept>

#include "database/Database.h"
#include "session/SessionManager.h"
#include "user/UserManager.h"
#include "match/MatchManager.h"
#include "room/RoomManager.h"

namespace animalfight {

std::unique_ptr<Database> ServerApp::database_;
std::mutex ServerApp::databaseMutex_;
std::unique_ptr<SessionManager> ServerApp::sessions_;
std::unique_ptr<UserManager> ServerApp::users_;
std::unique_ptr<MatchManager> ServerApp::matches_;
std::unique_ptr<RoomManager> ServerApp::rooms_;

void ServerApp::initialize() {
    sessions_ = std::make_unique<SessionManager>();
    users_ = std::make_unique<UserManager>();
    rooms_ = std::make_unique<RoomManager>();
    matches_ = std::make_unique<MatchManager>(*rooms_);
}

UserManager &ServerApp::users() { if (!users_) throw std::logic_error("ServerApp is not initialized"); return *users_; }
MatchManager &ServerApp::matches() { if (!matches_) throw std::logic_error("ServerApp is not initialized"); return *matches_; }
RoomManager &ServerApp::rooms() { if (!rooms_) throw std::logic_error("ServerApp is not initialized"); return *rooms_; }

Database &ServerApp::database() {
    std::lock_guard lock(databaseMutex_);
    if (!database_) database_ = std::make_unique<Database>();
    return *database_;
}

SessionManager &ServerApp::sessions() {
    if (!sessions_) throw std::logic_error("ServerApp is not initialized");
    return *sessions_;
}

}  // namespace animalfight

