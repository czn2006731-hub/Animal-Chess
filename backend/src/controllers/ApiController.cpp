#include "controllers/ApiController.h"

#include <optional>

#include "ServerApp.h"
#include "match/MatchManager.h"
#include "room/RoomManager.h"
#include "session/SessionManager.h"
#include "user/UserManager.h"

namespace animalfight {
namespace {

using Callback = std::function<void(const drogon::HttpResponsePtr &)>;

drogon::HttpResponsePtr result(bool ok, const std::string &message = "") {
    Json::Value json;
    json["ok"] = ok;
    if (!message.empty()) json["message"] = message;
    return drogon::HttpResponse::newHttpJsonResponse(json);
}

drogon::HttpResponsePtr forbidden() {
    auto response = result(false, "登录状态已失效，请重新登录。");
    response->setStatusCode(drogon::k401Unauthorized);
    return response;
}

std::optional<Session> sessionFor(const drogon::HttpRequestPtr &request) {
    const auto sid = request->getCookie("sid");
    return sid.empty() ? std::nullopt : ServerApp::sessions().find(sid);
}

Json::Value toJson(const UserProfile &user) {
    Json::Value json;
    json["id"] = user.id; json["username"] = user.username; json["score"] = user.score; json["level"] = user.level;
    json["wins"] = user.wins; json["losses"] = user.losses; json["draws"] = user.draws;
    return json;
}

void respondWithLogin(const UserProfile &user, Callback callback) {
    Json::Value json; json["ok"] = true; json["data"] = toJson(user);
    auto response = drogon::HttpResponse::newHttpJsonResponse(json);
    drogon::Cookie cookie("sid", ServerApp::sessions().create(user.id, user.username));
    cookie.setPath("/"); cookie.setHttpOnly(true); cookie.setMaxAge(60 * 60 * 24 * 7);
    response->addCookie(cookie);
    callback(response);
}

}  // namespace

void ApiController::registerUser(const drogon::HttpRequestPtr &request, Callback &&callback) {
    const auto body = request->getJsonObject();
    if (!body) return callback(result(false, "请求数据格式不正确。"));
    ServerApp::users().registerUser((*body)["username"].asString(), (*body)["password"].asString(),
        [callback = std::move(callback)](std::optional<UserProfile> user, const std::string &error) mutable {
            if (!user) return callback(result(false, error));
            respondWithLogin(*user, std::move(callback));
        });
}

void ApiController::login(const drogon::HttpRequestPtr &request, Callback &&callback) {
    const auto body = request->getJsonObject();
    if (!body) return callback(result(false, "请求数据格式不正确。"));
    ServerApp::users().login((*body)["username"].asString(), (*body)["password"].asString(),
        [callback = std::move(callback)](std::optional<UserProfile> user, const std::string &error) mutable {
            if (!user) return callback(result(false, error));
            respondWithLogin(*user, std::move(callback));
        });
}

void ApiController::logout(const drogon::HttpRequestPtr &request, Callback &&callback) {
    const auto sid = request->getCookie("sid");
    if (!sid.empty()) ServerApp::sessions().revoke(sid);
    auto response = result(true);
    drogon::Cookie cookie("sid", ""); cookie.setPath("/"); cookie.setMaxAge(0); response->addCookie(cookie);
    callback(response);
}

void ApiController::me(const drogon::HttpRequestPtr &request, Callback &&callback) {
    const auto session = sessionFor(request);
    if (!session) return callback(forbidden());
    ServerApp::users().findById(session->userId, [callback = std::move(callback)](std::optional<UserProfile> user, const std::string &error) mutable {
        if (!user) return callback(result(false, error));
        Json::Value json; json["ok"] = true; json["data"] = toJson(*user); callback(drogon::HttpResponse::newHttpJsonResponse(json));
    });
}

void ApiController::ranking(const drogon::HttpRequestPtr &, Callback &&callback) {
    ServerApp::users().ranking(50, [callback = std::move(callback)](std::vector<RankItem> ranks, const std::string &error) mutable {
        if (!error.empty()) return callback(result(false, error));
        Json::Value list(Json::arrayValue);
        for (const auto &rank : ranks) { auto item = toJson(rank); item["rank"] = rank.rank; item["winRate"] = rank.winRate; list.append(item); }
        Json::Value json; json["ok"] = true; json["data"] = list; callback(drogon::HttpResponse::newHttpJsonResponse(json));
    });
}

void ApiController::joinMatch(const drogon::HttpRequestPtr &request, Callback &&callback) {
    const auto session = sessionFor(request); if (!session) return callback(forbidden());
    const auto status = ServerApp::matches().join(session->userId);
    Json::Value data; data["searching"] = status.searching; data["matched"] = status.matched; if (!status.roomId.empty()) data["roomId"] = status.roomId;
    Json::Value json; json["ok"] = true; json["data"] = data; callback(drogon::HttpResponse::newHttpJsonResponse(json));
}

void ApiController::cancelMatch(const drogon::HttpRequestPtr &request, Callback &&callback) {
    const auto session = sessionFor(request); if (!session) return callback(forbidden());
    ServerApp::matches().cancel(session->userId); callback(result(true));
}

void ApiController::matchStatus(const drogon::HttpRequestPtr &request, Callback &&callback) {
    const auto session = sessionFor(request); if (!session) return callback(forbidden());
    const auto status = ServerApp::matches().status(session->userId);
    Json::Value data; data["searching"] = status.searching; data["matched"] = status.matched; if (!status.roomId.empty()) data["roomId"] = status.roomId;
    Json::Value json; json["ok"] = true; json["data"] = data; callback(drogon::HttpResponse::newHttpJsonResponse(json));
}

void ApiController::createPrivateRoom(const drogon::HttpRequestPtr &request, Callback &&callback) {
    const auto session = sessionFor(request); if (!session) return callback(forbidden());
    const auto roomId = ServerApp::rooms().create(session->userId, true);
    Json::Value data; data["roomId"] = roomId; Json::Value json; json["ok"] = true; json["data"] = data; callback(drogon::HttpResponse::newHttpJsonResponse(json));
}

void ApiController::joinPrivateRoom(const drogon::HttpRequestPtr &request, Callback &&callback) {
    const auto session = sessionFor(request); if (!session) return callback(forbidden());
    const auto body = request->getJsonObject(); if (!body) return callback(result(false, "请求数据格式不正确。"));
    const auto room = ServerApp::rooms().join((*body)["roomId"].asString(), session->userId);
    if (!room || !room->isPrivate) return callback(result(false, "房间号无效、已满员或不可加入。"));
    Json::Value data; data["roomId"] = room->id; Json::Value json; json["ok"] = true; json["data"] = data; callback(drogon::HttpResponse::newHttpJsonResponse(json));
}

void ApiController::rules(const drogon::HttpRequestPtr &, Callback &&callback) {
    Json::Value data; data["summary"] = "采用标准斗兽棋规则：象、狮、虎、豹、狼、狗、猫、鼠；鼠可下水，狮虎可跃河，进入对方兽穴即获胜。";
    Json::Value json; json["ok"] = true; json["data"] = data; callback(drogon::HttpResponse::newHttpJsonResponse(json));
}

}  // namespace animalfight
