#include "DeviceController.h"

// Allows to register the first device of a user
void DeviceController::registerDevice(const HttpRequestPtr& req,
                                      std::function<void(const HttpResponsePtr&)>&& callback) {
  auto json = req->getJsonObject();
  if (!json) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k400BadRequest);
    resp->setBody("Invalid JSON");
    callback(resp);
    return;
  }
  // Check if the user already has a device registered in the database
  std::cout << json->toStyledString() << std::endl;
  if (!json->isMember("user_id") || !(*json)["user_id"].isString()) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k400BadRequest);
    resp->setBody("Missing or invalid user_id");
    callback(resp);
    return;
  }
  if (!json->isMember("identity_pubkey") || !(*json)["identity_pubkey"].isString()) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k400BadRequest);
    resp->setBody("Missing or invalid identity_pubkey");
    callback(resp);
    return;
  }
  if (!json->isMember("push_token") || !(*json)["push_token"].isString()) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k400BadRequest);
    resp->setBody("Missing or invalid push_token");
    callback(resp);
    return;
  }
  if (!json->isMember("signed_prekey") || !(*json)["signed_prekey"].isObject()) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k400BadRequest);
    resp->setBody("Missing or invalid signed_prekey");
    callback(resp);
    return;
  }
  if (!json->isMember("one_time_prekeys") || !(*json)["one_time_prekeys"].isArray()) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k400BadRequest);
    resp->setBody("Missing or invalid one_time_prekeys");
    callback(resp);
    return;
  }
  if (!json->isMember("device_label") || !(*json)["device_label"].isString()) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k400BadRequest);
    resp->setBody("Missing or invalid device_label");
    callback(resp);
    return;
  }
  auto userId = (*json)["user_id"].asString();
  auto identityPubkey = (*json)["identity_pubkey"].asString();
  auto pushToken = (*json)["push_token"].asString();
  auto signedPrekey = (*json)["signed_prekey"];       // Object
  auto oneTimePrekeys = (*json)["one_time_prekeys"];  // Array
  auto deviceLabel = (*json)["device_label"].asString();
  auto dbClient = drogon::app().getDbClient();
  drogon::orm::Mapper<drogon_model::e2ee::Devices> mapper(dbClient);
  drogon::orm::Mapper<drogon_model::e2ee::Users> userMapper(dbClient);
  try {
    try {
      auto existingUser = userMapper.findByPrimaryKey(userId);
    } catch (const drogon::orm::UnexpectedRows &e) {
      auto resp = HttpResponse::newHttpResponse();
      resp->setStatusCode(k404NotFound);
      resp->setBody("User not found");
      callback(resp);
      return;
    }
    std::cout << "Checking existing devices for user_id: " << userId << std::endl;
    auto existingDevices = mapper.findBy(drogon::orm::Criteria(drogon_model::e2ee::Devices::Cols::_user_id, userId));
    std::cout << "Found " << existingDevices.size() << " existing devices for user_id: " << userId << std::endl;
    if (!existingDevices.empty()) {
        for (const auto& device : existingDevices) {
            std::cout << "Existing device ID: " << device.toJson().toStyledString() << ", Identity Pubkey: " << device.getIdentityPubkey() << std::endl;
        }
      auto resp = HttpResponse::newHttpResponse();
      resp->setStatusCode(k409Conflict);
      resp->setBody("User already has a registered device");
      callback(resp);
      return;
    }
    saveDeviceToDB(userId, identityPubkey, signedPrekey, oneTimePrekeys, pushToken, deviceLabel, req,
                   std::move(callback));
  } catch (const std::exception& e) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k500InternalServerError);
    resp->setBody("Database error: " + std::string(e.what()));
    callback(resp);
    return;
  }
}

void DeviceController::saveDeviceToDB(const std::string& userId, const std::string& identityPubkey,
                                      const Json::Value& signedPrekey, const Json::Value& oneTimePrekeys,
                                      const std::string& pushToken, const std::string& deviceLabel,
                                      const HttpRequestPtr& req,
                                      std::function<void(const HttpResponsePtr&)>&& callback) {
  auto dbClient = drogon::app().getDbClient();
  drogon::orm::Mapper<drogon_model::e2ee::Devices> deviceMapper(dbClient);

  try {
    // Save the device
    drogon_model::e2ee::Devices newDevice;
    newDevice.setUserId(userId);
    newDevice.setIdentityPubkey(identityPubkey);
    newDevice.setPushToken(pushToken);
    newDevice.setDeviceLabel(deviceLabel);
    newDevice.setLastSeen(trantor::Date::now());
    deviceMapper.insert(newDevice);
    std::cout << "Inserted device: " << newDevice.toJson().toStyledString() << std::endl;

    // Save the signed prekey
    saveSignedPrekeyToDB(newDevice.getValueOfId(), signedPrekey, oneTimePrekeys, req, std::move(callback));
  } catch (const std::exception& e) {
    // Delete the created device in case of error
    deviceMapper.deleteBy(drogon::orm::Criteria(drogon_model::e2ee::Devices::Cols::_identity_pubkey, identityPubkey));
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k500InternalServerError);
    resp->setBody("Database error: " + std::string(e.what()));
    callback(resp);
  }
}

void DeviceController::saveSignedPrekeyToDB(const std::string& deviceId, const Json::Value& signedPrekey,
                                            const Json::Value& oneTimePrekeys, const HttpRequestPtr& req,
                                            std::function<void(const HttpResponsePtr&)>&& callback) {
  auto dbClient = drogon::app().getDbClient();
  drogon::orm::Mapper<drogon_model::e2ee::SignedPrekeys> signedPrekeyMapper(dbClient);
  try {
    drogon_model::e2ee::SignedPrekeys newSignedPrekey;
    std::cout << "Inserting signed prekey for device ID: " << deviceId << std::endl;
    newSignedPrekey.setDeviceId(deviceId);
    // Parse JSON object to get key and signature
    newSignedPrekey.setKey(signedPrekey["key"].asString());
    newSignedPrekey.setSignature(signedPrekey["signature"].asString());
    signedPrekeyMapper.insert(newSignedPrekey);
    std::cout << "Inserted signed prekey: " << newSignedPrekey.toJson().toStyledString() << std::endl;
    saveOneTimePrekeysToDB(deviceId, oneTimePrekeys, req, std::move(callback));
  } catch (const std::exception& e) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k500InternalServerError);
    resp->setBody("Database error: " + std::string(e.what()));
    callback(resp);
  }
}

void DeviceController::saveOneTimePrekeysToDB(const std::string& deviceId, const Json::Value& oneTimePrekeys,
                                              const HttpRequestPtr& req,
                                              std::function<void(const HttpResponsePtr&)>&& callback) {
  auto dbClient = drogon::app().getDbClient();
  drogon::orm::Mapper<drogon_model::e2ee::OneTimePrekeys> oneTimePrekeyMapper(dbClient);
  try {
    for (const auto &otp : oneTimePrekeys) {
      drogon_model::e2ee::OneTimePrekeys newOneTimePrekey;
      newOneTimePrekey.setDeviceId(deviceId);
      newOneTimePrekey.setKey(otp.asString());
      oneTimePrekeyMapper.insert(newOneTimePrekey);
      std::cout << "Inserted one-time prekey " << newOneTimePrekey.toJson().toStyledString() << std::endl;
    }
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k201Created);
    resp->setBody("Device and keys registered successfully");
    std::cout << "Inserted " << oneTimePrekeys.size() << " one-time prekeys for device ID: " << deviceId
              << std::endl;
    callback(resp);
  } catch (const std::exception& e) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k500InternalServerError);
    resp->setBody("Database error: " + std::string(e.what()));
    callback(resp);
  }
}