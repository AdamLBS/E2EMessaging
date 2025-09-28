#include "AuthController.h"

// Registers the user and the device
void AuthController::registerUser(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) {}

// Checks that the user controls the private key associated to the device, by receiving the device_id generating a nounce and send it
void AuthController::requestAuth(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) {}

// The client signs the nounce with the private key and we check if it matches

void AuthController::verifyAuth(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) {}

// Allows the client to renew the JWT token
void AuthController::renewAuth(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) {}
