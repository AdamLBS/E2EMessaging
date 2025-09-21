cmake_minimum_required(VERSION 3.20)
project(e2ee-backend LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

find_package(Drogon CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
# find_package(PkgConfig REQUIRED)   # (inutile ici, tu peux enlever)
find_package(unofficial-sodium CONFIG REQUIRED)
find_package(jwt-cpp CONFIG REQUIRED)
find_package(PostgreSQL REQUIRED)          # libpq
find_package(libpqxx CONFIG REQUIRED)      # <-- au lieu de pqxx
find_package(hiredis CONFIG REQUIRED)      # <-- ajouté
find_package(redis++ CONFIG REQUIRED)      # <-- au lieu de unofficial-redis-plus-plus

add_executable(e2ee-backend
  src/main.cpp
)

target_link_libraries(e2ee-backend PRIVATE
  Drogon::Drogon
  OpenSSL::SSL OpenSSL::Crypto
  unofficial-sodium::sodium
  jwt-cpp::jwt-cpp
  PostgreSQL::PostgreSQL
  libpqxx::pqxx                # <-- au lieu de pqxx::pqxx
  hiredis::hiredis             # <-- ajouté
  redis++::redis++             # <-- au lieu de unofficial::redis-plus-plus::redis-plus-plus
)


add_custom_command(TARGET e2ee-backend POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_SOURCE_DIR}/config.json
    $<TARGET_FILE_DIR:e2ee-backend>/config.json
)
