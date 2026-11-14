#pragma once

#include <drogon/HttpController.h>

namespace animalfight {

class ApiController : public drogon::HttpController<ApiController> {
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(ApiController::registerUser, "/api/auth/register", drogon::Post);
    ADD_METHOD_TO(ApiController::login, "/api/auth/login", drogon::Post);
    ADD_METHOD_TO(ApiController::logout, "/api/auth/logout", drogon::Post);
    ADD_METHOD_TO(ApiController::me, "/api/me", drogon::Get);
    ADD_METHOD_TO(ApiController::ranking, "/api/ranking", drogon::Get);
    ADD_METHOD_TO(ApiController::joinMatch, "/api/match/join", drogon::Post);
    ADD_METHOD_TO(ApiController::cancelMatch, "/api/match/cancel", drogon::Post);
    ADD_METHOD_TO(ApiController::matchStatus, "/api/match/status", drogon::Get);
    ADD_METHOD_TO(ApiController::createPrivateRoom, "/api/private-room/create", drogon::Post);
    ADD_METHOD_TO(ApiController::joinPrivateRoom, "/api/private-room/join", drogon::Post);
    ADD_METHOD_TO(ApiController::rules, "/api/rules", drogon::Get);
    METHOD_LIST_END

    void registerUser(const drogon::HttpRequestPtr &, std::function<void(const drogon::HttpResponsePtr &)> &&);
    void login(const drogon::HttpRequestPtr &, std::function<void(const drogon::HttpResponsePtr &)> &&);
    void logout(const drogon::HttpRequestPtr &, std::function<void(const drogon::HttpResponsePtr &)> &&);
    void me(const drogon::HttpRequestPtr &, std::function<void(const drogon::HttpResponsePtr &)> &&);
    void ranking(const drogon::HttpRequestPtr &, std::function<void(const drogon::HttpResponsePtr &)> &&);
    void joinMatch(const drogon::HttpRequestPtr &, std::function<void(const drogon::HttpResponsePtr &)> &&);
    void cancelMatch(const drogon::HttpRequestPtr &, std::function<void(const drogon::HttpResponsePtr &)> &&);
    void matchStatus(const drogon::HttpRequestPtr &, std::function<void(const drogon::HttpResponsePtr &)> &&);
    void createPrivateRoom(const drogon::HttpRequestPtr &, std::function<void(const drogon::HttpResponsePtr &)> &&);
    void joinPrivateRoom(const drogon::HttpRequestPtr &, std::function<void(const drogon::HttpResponsePtr &)> &&);
    void rules(const drogon::HttpRequestPtr &, std::function<void(const drogon::HttpResponsePtr &)> &&);
};

}  // namespace animalfight
