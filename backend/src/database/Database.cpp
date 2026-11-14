#include "database/Database.h"

#include <drogon/drogon.h>
#include <stdexcept>

namespace animalfight {

Database::Database() : client_(drogon::app().getDbClient("animalfight_mysql")) {
    if (!client_) throw std::runtime_error("MySQL client 'animalfight_mysql' is not configured");
}

const drogon::orm::DbClientPtr &Database::client() const noexcept { return client_; }

}  // namespace animalfight

