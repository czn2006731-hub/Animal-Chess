#include "user/UserManager.h"

#include <algorithm>

#include <drogon/orm/Exception.h>
#include <drogon/orm/Field.h>
#include <drogon/orm/Result.h>
#include <drogon/orm/Row.h>

#include "ServerApp.h"
#include "utils/Utils.h"

namespace animalfight {
namespace {

UserProfile profileFromRow(const drogon::orm::Row &row) {
    return {row["id"].as<int>(), row["username"].as<std::string>(), row["score"].as<int>(), row["level"].as<int>(), row["wins"].as<int>(), row["losses"].as<int>(), row["draws"].as<int>()};
}

}  // namespace

void UserManager::registerUser(const std::string &username, const std::string &password, UserCallback callback) const {
    if (!Utils::isValidUsername(username)) return callback(std::nullopt, "用户名需为 3–20 位字母、数字或下划线。");
    if (password.size() < 8 || password.size() > 128) return callback(std::nullopt, "密码长度需为 8–128 位。");
    const auto salt = Utils::randomToken(16);
    const auto hash = Utils::sha256(salt + password);
    ServerApp::database().client()->execSqlAsync(
        "INSERT INTO users (username, password_hash, password_salt) VALUES (?, ?, ?)",
        [username, callback](const drogon::orm::Result &) {
            ServerApp::database().client()->execSqlAsync(
                "SELECT id, username, score, level, wins, losses, draws FROM users WHERE username = ? LIMIT 1",
                [callback](const drogon::orm::Result &result) {
                    if (result.empty()) callback(std::nullopt, "账号创建后读取失败。");
                    else callback(profileFromRow(result[0]), "");
                },
                [callback](const drogon::orm::DrogonDbException &) { callback(std::nullopt, "数据库暂时不可用。"); }, username);
        },
        [callback](const drogon::orm::DrogonDbException &) { callback(std::nullopt, "该用户名已被使用，或数据库暂时不可用。"); }, username, hash, salt);
}

void UserManager::login(const std::string &username, const std::string &password, UserCallback callback) const {
    ServerApp::database().client()->execSqlAsync(
        "SELECT id, username, password_hash, password_salt, score, level, wins, losses, draws FROM users WHERE username = ? LIMIT 1",
        [password, callback](const drogon::orm::Result &result) {
            if (result.empty()) return callback(std::nullopt, "用户名或密码错误。");
            const auto &row = result[0];
            const auto expected = row["password_hash"].as<std::string>();
            const auto actual = Utils::sha256(row["password_salt"].as<std::string>() + password);
            if (expected != actual) return callback(std::nullopt, "用户名或密码错误。");
            callback(profileFromRow(row), "");
        },
        [callback](const drogon::orm::DrogonDbException &) { callback(std::nullopt, "数据库暂时不可用。"); }, username);
}

void UserManager::findById(int userId, UserCallback callback) const {
    ServerApp::database().client()->execSqlAsync(
        "SELECT id, username, score, level, wins, losses, draws FROM users WHERE id = ? LIMIT 1",
        [callback](const drogon::orm::Result &result) { if (result.empty()) callback(std::nullopt, "用户不存在。"); else callback(profileFromRow(result[0]), ""); },
        [callback](const drogon::orm::DrogonDbException &) { callback(std::nullopt, "数据库暂时不可用。"); }, userId);
}

void UserManager::ranking(int limit, RankingCallback callback) const {
    limit = std::clamp(limit, 1, 50);
    ServerApp::database().client()->execSqlAsync(
        "SELECT id, username, score, level, wins, losses, draws FROM users ORDER BY score DESC, id ASC LIMIT ?",
        [callback](const drogon::orm::Result &result) {
            std::vector<RankItem> ranks;
            int position = 1;
            for (const auto &row : result) {
                const auto user = profileFromRow(row);
                const auto total = user.wins + user.losses + user.draws;
                ranks.push_back({user, position++, total == 0 ? 0 : (user.wins * 100 / total)});
            }
            callback(std::move(ranks), "");
        },
        [callback](const drogon::orm::DrogonDbException &) { callback({}, "数据库暂时不可用。"); }, limit);
}

}  // namespace animalfight
