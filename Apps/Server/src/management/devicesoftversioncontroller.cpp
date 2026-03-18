#include "devicesoftversioncontroller.hpp"

#include <Components/Logger/Logger.h>

#include "../endpoint/servercommon.hpp"

DeviceSoftVersionController::DeviceSoftVersionController(const std::shared_ptr<DetectorCommandProcessor> &detectorCommandProcessor) :
    drogon::HttpController<DeviceSoftVersionController, false>(),
    m_detectorCommandProcessor {detectorCommandProcessor}
{

}

void DeviceSoftVersionController::processGetSoftVersion(
    const drogon::HttpRequestPtr &req,
    ResponseCallback_t &&callback,
    const std::string &deviceUuid)
{
    auto version = m_detectorCommandProcessor->getCurrentVersion();
    auto pResponse = drogon::HttpResponse::newHttpResponse(drogon::k200OK, drogon::CT_TEXT_PLAIN);
    pResponse->setBody(version);
    callback(pResponse);
}

void DeviceSoftVersionController::processAddVersion(
    const drogon::HttpRequestPtr &req,
    ResponseCallback_t &&callback,
    const std::string &versionUuid,
    const std::string &versionName)
{
    // 32 -- SHA-256 or MD5 HEX hash byte count
    if (versionName.empty() || versionUuid.empty() || (versionUuid.size() != 32)) {
        auto pResponse = drogon::HttpResponse::newHttpResponse(drogon::k400BadRequest, drogon::CT_NONE);
        callback(pResponse);
        return;
    }

    // Receive file
    drogon::MultiPartParser fileUpload;
    if (fileUpload.parse(req) != 0) {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k406NotAcceptable);
        resp->setBody("Failed to process file");
        callback(resp);
        return;
    }

    // Checkup
    auto &files = fileUpload.getFiles();
    if (files.empty()) {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k400BadRequest);
        resp->setBody("No file uploaded");
        callback(resp);
        return;
    }

    // Try to save file
    auto &file = files[0];
    auto resfilePath = Common::DirectoryManager::getDirectoryStatic(ServerCommon::DIRTYPE_SOFT_VERSIONS);
    if (file.save(resfilePath) != 0) {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k500InternalServerError);
        resp->setBody("Failed to save file");
        callback(resp);
    }
    std::filesystem::rename(resfilePath / file.getFileName(), resfilePath / (versionName + "_" + versionUuid));
    if (!m_detectorCommandProcessor->registerSoftVersion(versionName, versionUuid)) {
        std::filesystem::remove(resfilePath / (versionName + "_" + versionUuid));
        auto pResponse = drogon::HttpResponse::newHttpResponse(drogon::k500InternalServerError, drogon::CT_TEXT_PLAIN);
        COMPLOG_ERROR("Failed to add soft version:", m_detectorCommandProcessor->getLastErrorText());
        callback(pResponse);
        return;
    }
    COMPLOG_OK("Added version:", versionName, versionUuid);

    auto pResponse = drogon::HttpResponse::newHttpResponse(drogon::k200OK, drogon::CT_NONE);
    callback(pResponse);
}

void DeviceSoftVersionController::processSetSoftVersion(
    const drogon::HttpRequestPtr &req,
    ResponseCallback_t &&callback,
    const std::string& deviceUuid,
    const std::string& versionUuid)
{
    auto isSucceed = m_detectorCommandProcessor->setSoftVersion(versionUuid);

    if (!isSucceed.has_value()) {
        auto pResponse = drogon::HttpResponse::newHttpResponse(drogon::k404NotFound, drogon::CT_NONE);
        callback(pResponse);
        return;
    }

    if (!isSucceed.value()) {
        auto pResponse = drogon::HttpResponse::newHttpResponse(drogon::k500InternalServerError, drogon::CT_TEXT_PLAIN);
        pResponse->setBody(m_detectorCommandProcessor->getLastErrorText());
        callback(pResponse);
        return;
    }

    auto pResponse = drogon::HttpResponse::newHttpResponse(drogon::k200OK, drogon::CT_NONE);
    callback(pResponse);
}

void DeviceSoftVersionController::processRemoveVersion(
    const drogon::HttpRequestPtr &req,
    ResponseCallback_t &&callback,
    const std::string &versionUuid)
{
    if (versionUuid.empty()) {
        auto pResponse = drogon::HttpResponse::newHttpResponse(drogon::k406NotAcceptable, drogon::CT_NONE);
        callback(pResponse);
        return;
    }

    auto isSucceed = m_detectorCommandProcessor->removeSoftVersion(versionUuid);
    if (!isSucceed) {
        auto pResponse = drogon::HttpResponse::newHttpResponse(drogon::k500InternalServerError, drogon::CT_TEXT_PLAIN);
        pResponse->setBody(m_detectorCommandProcessor->getLastErrorText());
        callback(pResponse);
        return;
    }
    auto pResponse = drogon::HttpResponse::newHttpResponse(drogon::k200OK, drogon::CT_NONE);
    callback(pResponse);
}
