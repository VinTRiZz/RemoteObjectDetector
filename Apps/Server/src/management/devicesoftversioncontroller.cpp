#include "devicesoftversioncontroller.hpp"

#include <Components/Logger/Logger.h>
#include <Components/Common/DirectoryManager.h>
#include <Components/Filework/Common.h>

#include "../endpoint/servercommon.hpp"

DeviceSoftVersionController::DeviceSoftVersionController(ServerEventLogger &eventLogger) :
    drogon::HttpController<DeviceSoftVersionController, false>(),
    m_eventLogger {eventLogger}
{

}

void DeviceSoftVersionController::getSoftVersion(const drogon::HttpRequestPtr &req, std::function<void (const drogon::HttpResponsePtr &)> &&callback)
{
    if (m_versionGetter) {
        auto version = m_versionGetter();
        auto pResponse = drogon::HttpResponse::newHttpResponse(drogon::k200OK, drogon::CT_TEXT_PLAIN);
        pResponse->setBody(version);
        callback(pResponse);
        return;
    }
    auto pResponse = drogon::HttpResponse::newHttpResponse(drogon::k501NotImplemented, drogon::CT_NONE);
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
        resp->setStatusCode(drogon::k400BadRequest);
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
    COMPLOG_OK("Added version:", versionNameString, versionHash);

    m_eventLogger.logEvent(ServerCommon::AddedVersion, versionHash);
    auto pResponse = drogon::HttpResponse::newHttpResponse(drogon::k200OK, drogon::CT_NONE);
    callback(pResponse);
}

void DeviceSoftVersionController::setSoftVersion(const drogon::HttpRequestPtr &req, std::function<void (const drogon::HttpResponsePtr &)> &&callback)
{
    auto versionNameString = req->getParameter("version");
    if (!m_versionUpdater) {
        auto pResponse = drogon::HttpResponse::newHttpResponse(drogon::k501NotImplemented, drogon::CT_NONE);
        callback(pResponse);
        return;
    }

    std::string versionFilename;
    auto versionsDir = Common::DirectoryManager::getDirectoryStatic(ServerCommon::DIRTYPE_SOFT_VERSIONS);
    for (auto& filename : Filework::Common::getContentNames(versionsDir)) {
        auto versionString = filename;
        versionString.erase(versionString.find_first_of("_"), -1);
        if (versionString != versionNameString) {
            continue;
        }
        versionFilename = (versionsDir / filename);
        break;
    }

    if (versionFilename.empty()) {
        auto pResponse = drogon::HttpResponse::newHttpResponse(drogon::k404NotFound, drogon::CT_NONE);
        callback(pResponse);
        return;
    }

    auto isSucceed = m_versionUpdater(versionFilename);
    if (isSucceed) {
        m_eventLogger.logEvent(ServerCommon::SetVersion, versionFilename);
    }

    auto pResponse = drogon::HttpResponse::newHttpResponse(isSucceed ? drogon::k200OK : drogon::k500InternalServerError, drogon::CT_NONE);
    callback(pResponse);
    return;
}

void DeviceSoftVersionController::removeVersion(const drogon::HttpRequestPtr &req, std::function<void (const drogon::HttpResponsePtr &)> &&callback)
{
    auto versionNameString = req->getParameter("version");
    auto versionsDir = Common::DirectoryManager::getDirectoryStatic(ServerCommon::DIRTYPE_SOFT_VERSIONS);
    for (auto& filename : Filework::Common::getContentNames(versionsDir)) {
        auto versionString = filename;
        versionString.erase(versionString.find_first_of("_"), -1);
        if (versionString != versionNameString) {
            continue;
        }
        std::filesystem::remove(versionsDir / filename);

        auto versionHash = filename;
        versionHash.erase(0, versionHash.find_last_of("_") + 1);
        m_eventLogger.logEvent(ServerCommon::RemovedVersion);
        auto pResponse = drogon::HttpResponse::newHttpResponse(drogon::k200OK, drogon::CT_NONE);
        callback(pResponse);
        return;
    }
    auto pResponse = drogon::HttpResponse::newHttpResponse(drogon::k501NotImplemented, drogon::CT_NONE);
    callback(pResponse);
}

void DeviceSoftVersionController::setVersionGetter(std::function<std::string ()> &&versionGetter)
{
    m_versionGetter = std::move(versionGetter);
}

void DeviceSoftVersionController::setUpdater(std::function<bool (const std::string &)> &&versionUpdater)
{
    m_versionUpdater = std::move(versionUpdater);
}
