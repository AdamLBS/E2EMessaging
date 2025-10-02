#include "AuthController.h"

// Registers the user and the device
void AuthController::registerUser(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) {
    // Parse the request body to get user and device information
    auto json = req->getJsonObject();
    if (!json || !json->isMember("username")) {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k400BadRequest);
        resp->setBody("Invalid request body");
        callback(resp);
        return;
    }
    std::string username = (*json)["username"].asString();
    drogon_model::e2ee::Users user;
    user.setUsername(username);
    user.setCreatedAt(trantor::Date::now());
    auto dbClient = drogon::app().getDbClient();
    try {
        auto mapper = drogon::orm::Mapper<drogon_model::e2ee::Users>(dbClient);
        mapper.insert(user);
    } catch (const std::exception& e) {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k500InternalServerError);
        resp->setBody("Database error: " + std::string(e.what()));
        callback(resp);
        return;
    }
    bool success = true; // replace with actual database operation result
    if (success) {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k200OK);
        resp->setBody(user.toJson().toStyledString());
        callback(resp);
    } else {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k500InternalServerError);
        resp->setBody("Failed to register user");
        callback(resp);
        return;
    }
}

// Checks that the user controls the private key associated to the device, by receiving the device_id generating a nounce and send it
void AuthController::requestAuth(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) {}

// The client signs the nounce with the private key and we check if it matches

void AuthController::verifyAuth(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) {}

// Allows the client to renew the JWT token
void AuthController::renewAuth(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) {}
