#pragma once

#include <drogon/orm/DbClient.h>

namespace animalfight {

class Database {
  public:
    Database();
    [[nodiscard]] const drogon::orm::DbClientPtr &client() const noexcept;

  private:
    drogon::orm::DbClientPtr client_;
};

}  // namespace animalfight

