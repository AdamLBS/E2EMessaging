#pragma once
#include <drogon/HttpController.h>

using namespace drogon;

class AuthController : public drogon::HttpController<AuthController>
{
    public:
        // Routes
        METHOD_LIST_BEGIN
            ADD_METHOD_TO(AuthController::registerUser, "/register", Post);
            ADD_METHOD_TO(AuthController::requestAuth, "/auth/request", Post);
            ADD_METHOD_TO(AuthController::verifyAuth, "/auth/verify", Post);
            ADD_METHOD_TO(AuthController::renewAuth, "/auth/renew", Post);
        METHOD_LIST_END
    
        //Handlers
        void registerUser(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);
        void requestAuth(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);
        void verifyAuth(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);
        void renewAuth(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);
};