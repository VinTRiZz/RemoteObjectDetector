#include "detectorsoftwarecontroller.hpp"

#include <Components/Logger/Logger.h>
#include <Components/Common/DirectoryManager.h>

#include <nlohmann/json.hpp>

DetectorSoftwareController::DetectorSoftwareController() :
    drogon::HttpController<DetectorSoftwareController, false>()
{
    m_detectorCommandProcessor = std::make_shared<DetectorCommandProcessor>(m_deviceSoftwareManager);
}

void DetectorSoftwareController::processGetExistingVersions(const drogon::HttpRequestPtr &req, ResponseCallback_t &&callback)
{
    nlohmann::json res;
    for (auto& vers : m_deviceSoftwareManager.getExistingVersions()) {
        res.push_back(vers);
    }
    sendJsonMessage(drogon::k200OK, res.dump(), std::move(callback));
}

void DetectorSoftwareController::processGetSoftVersion(
    const drogon::HttpRequestPtr &req,
    ResponseCallback_t &&callback,
    const DataObjects::id_t::type &deviceId)
{
    auto version = m_detectorCommandProcessor->getCurrentVersion(deviceId);
    sendTextMessage(drogon::k200OK, version, std::move(callback));
}

void DetectorSoftwareController::processAddVersion(
    const drogon::HttpRequestPtr &req,
    ResponseCallback_t &&callback)
{
    // Receive file
    drogon::MultiPartParser fileUpload;
    if (fileUpload.parse(req) != 0) {
        sendTextMessage(drogon::k406NotAcceptable, "Failed to process file", std::move(callback));
        return;
    }

    // Checkup
    auto &files = fileUpload.getFiles();
    if (files.empty()) {
        sendTextMessage(drogon::k400BadRequest, "No file uploaded", std::move(callback));
        return;
    }

    // Try to save file
    auto &file = files[0];
    auto resfilePath = Common::DirectoryManager::getDirectoryStatic(Common::DirectoryManager::Temporary);
    if (file.save(resfilePath) != 0) {
        sendTextMessage(drogon::k500InternalServerError, "Failed to save file", std::move(callback));
        return;
    }

    if (!m_deviceSoftwareManager.addVersionFile(resfilePath / file.getFileName(), file.getMd5())) {
        std::filesystem::remove(resfilePath / file.getFileName());
        sendTextMessage(drogon::k500InternalServerError, "Version registration failed", std::move(callback));
        return;
    }
    sendTextMessage(drogon::k200OK, "File saved", std::move(callback));
}

void DetectorSoftwareController::processSetSoftVersion(
    const drogon::HttpRequestPtr &req,
    ResponseCallback_t &&callback,
    const DataObjects::id_t::type& deviceId,
    const DataObjects::id_t::type& versionId)
{
    auto isSucceed = m_detectorCommandProcessor->setSoftVersion(deviceId, versionId);
    if (!isSucceed.has_value()) {
        sendTextMessage(drogon::k404NotFound, "Version not found", std::move(callback));
        return;
    }

    if (!isSucceed.value()) {
        sendTextMessage(drogon::k500InternalServerError, "Version set failed", std::move(callback));
        return;
    }

    sendTextMessage(drogon::k200OK, "Version set", std::move(callback));
}

void DetectorSoftwareController::processRemoveVersion(
    const drogon::HttpRequestPtr &req,
    ResponseCallback_t &&callback,
    const DataObjects::id_t::type &versionId)
{
    auto isSucceed = m_deviceSoftwareManager.removeVersion(versionId);
    if (!isSucceed) {
        sendTextMessage(drogon::k500InternalServerError, "Version faild to remove", std::move(callback));
        return;
    }
    sendTextMessage(drogon::k200OK, "Version removed", std::move(callback));
}

std::shared_ptr<DetectorCommandProcessor> DetectorSoftwareController::getDetectorCommandProcessor() const
{
    return m_detectorCommandProcessor;
}

