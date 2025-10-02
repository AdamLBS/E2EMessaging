#pragma once
#include <drogon/HttpController.h>
#include <models/Devices.h>
#include <models/OneTimePrekeys.h>
#include <models/SignedPrekeys.h>
#include <models/Users.h>

using namespace drogon;
using namespace drogon_model::e2ee;

class DeviceController : public drogon::HttpController<DeviceController> {
 public:
  // Routes
  METHOD_LIST_BEGIN
  ADD_METHOD_TO(DeviceController::registerDevice, "/device/register", Post);
  METHOD_LIST_END

  // Handlers
  void registerDevice(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);

 private:
  void saveDeviceToDB(const std::string& userId, const std::string& identityPubkey, const Json::Value& signedPrekey,
                      const Json::Value& oneTimePrekeys, const std::string& pushToken, const std::string& deviceLabel,
                      const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);
  void saveSignedPrekeyToDB(const std::string& deviceId, const Json::Value& signedPrekey,
                            const Json::Value& oneTimePrekeys, const HttpRequestPtr& req,
                            std::function<void(const HttpResponsePtr&)>&& callback);
  void saveOneTimePrekeysToDB(const std::string& deviceId, const Json::Value& oneTimePrekeys, const HttpRequestPtr& req,
                              std::function<void(const HttpResponsePtr&)>&& callback);
};