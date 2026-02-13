#include "devicesoftversioncontroller.hpp"

DeviceSoftVersionController::DeviceSoftVersionController(ServerEventLogger &eventLogger) :
    drogon::HttpController<DeviceSoftVersionController, false>(),
    m_eventLogger {eventLogger}
{

}

void DeviceSoftVersionController::getSoftVersion(const drogon::HttpRequestPtr &req, std::function<void (const drogon::HttpResponsePtr &)> &&callback)
{

}

void DeviceSoftVersionController::addVersion(const drogon::HttpRequestPtr &req, std::function<void (const drogon::HttpResponsePtr &)> &&callback)
{

}

void DeviceSoftVersionController::setSoftVersion(const drogon::HttpRequestPtr &req, std::function<void (const drogon::HttpResponsePtr &)> &&callback)
{

}

void DeviceSoftVersionController::removeVersion(const drogon::HttpRequestPtr &req, std::function<void (const drogon::HttpResponsePtr &)> &&callback)
{

}




/*
    // GET /api/device/status?dev=...
    void getDeviceStatus(const HttpRequestPtr &req,
                         std::function<void(const HttpResponsePtr &)> &&callback)
    {
        auto dev = req->getParameter("dev");
        if (dev.empty())
        {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k400BadRequest);
            resp->setBody("Missing 'dev' parameter");
            callback(resp);
            return;
        }

        Json::Value json;
        json["device"] = dev;
        json["status"] = "online";
        json["timestamp"] = trantor::Date::now().toCustomedFormattedString();

        auto resp = HttpResponse::newHttpJsonResponse(json);
        callback(resp);
    }

    // GET /api/update?version=...
    void getUpdateFile(const HttpRequestPtr &req,
                       std::function<void(const HttpResponsePtr &)> &&callback)
    {
        auto version = req->getParameter("version");
        if (version.empty())
        {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k400BadRequest);
            resp->setBody("Missing 'version' parameter");
            callback(resp);
            return;
        }

        // Формируем путь к файлу: ./updates/update_<version>.bin
        fs::path filePath = fs::current_path() / "updates" / ("update_" + version + ".bin");
        if (!fs::exists(filePath))
        {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k404NotFound);
            resp->setBody("Update file not found for version " + version);
            callback(resp);
            return;
        }

        // Отправляем файл
        auto resp = HttpResponse::newFileResponse(filePath.string());
        callback(resp);
    }

    // PUT /api/update?version=2.0
    void putUpdateFile(const HttpRequestPtr &req,
                       std::function<void(const HttpResponsePtr &)> &&callback)
    {
        auto version = req->getParameter("version");
        if (version != "2.0")
        {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k400BadRequest);
            resp->setBody("Only version=2.0 is supported for PUT");
            callback(resp);
            return;
        }

        fs::path filePath = fs::current_path() / "myfile.bin";
        if (!fs::exists(filePath))
        {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k404NotFound);
            resp->setBody("myfile.bin not found");
            callback(resp);
            return;
        }

        // Проверяем размер файла (от 1 до 20 Мбайт)
        std::uintmax_t fileSize = fs::file_size(filePath);
        const std::uintmax_t minSize = 1 * 1024 * 1024;     // 1 МБ
        const std::uintmax_t maxSize = 20 * 1024 * 1024;    // 20 МБ

        if (fileSize < minSize || fileSize > maxSize)
        {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k400BadRequest);
            resp->setBody("File size must be between 1 and 20 MB (current: " +
                          std::to_string(fileSize) + " bytes)");
            callback(resp);
            return;
        }

        // Отправляем файл
        auto resp = HttpResponse::newFileResponse(filePath.string());
        callback(resp);
    }

  */
