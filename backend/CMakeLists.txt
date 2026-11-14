cmake_minimum_required(VERSION 3.16)
project(animalfight VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Drogon CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)

add_executable(animalfight
  src/main.cpp
  src/ServerApp.cpp
  src/database/Database.cpp
  src/user/UserManager.cpp
  src/session/SessionManager.cpp
  src/match/MatchManager.cpp
  src/room/RoomManager.cpp
  src/utils/Utils.cpp
  src/controllers/ApiController.cpp
)

target_include_directories(animalfight PRIVATE src)
target_link_libraries(animalfight PRIVATE Drogon::Drogon OpenSSL::Crypto)
target_compile_options(animalfight PRIVATE -Wall -Wextra -Wpedantic)

