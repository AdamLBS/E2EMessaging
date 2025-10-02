cmake_minimum_required(VERSION 3.20)
project(e2ee-backend LANGUAGES CXX)

# ---- C++ standard ----
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# ---- Dépendances ----
find_package(Drogon CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(unofficial-sodium CONFIG REQUIRED)
find_package(jwt-cpp CONFIG REQUIRED)
find_package(PostgreSQL REQUIRED)          # libpq
find_package(libpqxx CONFIG REQUIRED)
find_package(hiredis CONFIG REQUIRED)
find_package(redis++ CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

# ---- Sources ----

file(GLOB_RECURSE SOURCES
  src/*.cpp
  src/*.cc
  models/*.cc
  models/*.cpp
)

# ---- Exécutable ----
add_executable(e2ee-backend ${SOURCES})

# ---- Linking ----

target_include_directories(e2ee-backend PRIVATE
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(e2ee-backend PRIVATE
  Drogon::Drogon
  OpenSSL::SSL OpenSSL::Crypto
  unofficial-sodium::sodium
  jwt-cpp::jwt-cpp
  PostgreSQL::PostgreSQL
  libpqxx::pqxx
  hiredis::hiredis
  redis++::redis++_static   # redis++ avec vcpkg
  spdlog::spdlog            # logging
)

# ---- Copier config.json à côté du binaire ----
add_custom_command(TARGET e2ee-backend POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_SOURCE_DIR}/config.json
    $<TARGET_FILE_DIR:e2ee-backend>/config.json
)
