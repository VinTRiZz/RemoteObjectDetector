#include "devicesoftversioncontroller.hpp"

#include <Components/Logger/Logger.h>

#include "../endpoint/servercommon.hpp"

DeviceSoftVersionController::DeviceSoftVersionController(const std::shared_ptr<DetectorCommandProcessor> &detectorCommandProcessor) :
    drogon::HttpController<DeviceSoftVersionController, false>(),
    m_detectorCommandProcessor {detectorCommandProcessor}
{

}

void DeviceSoftVersionController::getSoftVersion(const drogon::HttpRequestPtr &req, std::function<void (const drogon::HttpResponsePtr &)> &&callback)
{
    auto version = m_detectorCommandProcessor->getCurrentVersion();
    auto pResponse = drogon::HttpResponse::newHttpResponse(drogon::k200OK, drogon::CT_TEXT_PLAIN);
    pResponse->setBody(version);
    callback(pResponse);
}

void DeviceSoftVersionController::addVersion(const drogon::HttpRequestPtr &req, std::function<void (const drogon::HttpResponsePtr &)> &&callback)
{
    auto versionNameString = req->getParameter("version");
    auto versionHash = req->getParameter("hash");

    // 32 -- SHA-256 or MD5 HEX hash byte count
    if (versionNameString.empty() || versionHash.empty() || (versionHash.size() != 32)) {
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
    std::filesystem::rename(resfilePath / file.getFileName(), resfilePath / (versionNameString + "_" + versionHash));
    if (!m_detectorCommandProcessor->registerSoftVersion(versionNameString, versionHash)) {
        std::filesystem::remove(resfilePath / (versionNameString + "_" + versionHash));
        auto pResponse = drogon::HttpResponse::newHttpResponse(drogon::k500InternalServerError, drogon::CT_TEXT_PLAIN);
        COMPLOG_ERROR("Failed to add soft version:", m_detectorCommandProcessor->getLastErrorText());
        callback(pResponse);
        return;
    }
    COMPLOG_OK("Added version:", versionNameString, versionHash);

    auto pResponse = drogon::HttpResponse::newHttpResponse(drogon::k200OK, drogon::CT_NONE);
    callback(pResponse);
}

void DeviceSoftVersionController::setSoftVersion(const drogon::HttpRequestPtr &req, std::function<void (const drogon::HttpResponsePtr &)> &&callback)
{
    auto versionNameString = req->getParameter("version");
    auto isSucceed = m_detectorCommandProcessor->setSoftVersion(versionNameString);

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

void DeviceSoftVersionController::removeVersion(const drogon::HttpRequestPtr &req, std::function<void (const drogon::HttpResponsePtr &)> &&callback)
{
    auto versionNameString = req->getParameter("version");
    if (versionNameString.empty()) {
        auto pResponse = drogon::HttpResponse::newHttpResponse(drogon::k406NotAcceptable, drogon::CT_NONE);
        callback(pResponse);
        return;
    }

    auto isSucceed = m_detectorCommandProcessor->removeSoftVersion(versionNameString);
    if (!isSucceed) {
        auto pResponse = drogon::HttpResponse::newHttpResponse(drogon::k500InternalServerError, drogon::CT_TEXT_PLAIN);
        pResponse->setBody(m_detectorCommandProcessor->getLastErrorText());
        callback(pResponse);
        return;
    }
    auto pResponse = drogon::HttpResponse::newHttpResponse(drogon::k200OK, drogon::CT_NONE);
    callback(pResponse);
}
