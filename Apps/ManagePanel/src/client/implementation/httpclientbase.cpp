#include "httpclientbase.hpp"

#include <QNetworkRequest>
#include <QNetworkReply>

#include <QNetworkCookieJar>

#include <QFile>
#include <QFileInfo>

#include "httpcommon.hpp"

namespace Web::Implementation
{

HTTPClientBase::HTTPClientBase(QObject *parent)
    : QObject{parent}
{

}

void HTTPClientBase::setServer(const QString &serverAddress)
{
    m_serverAddress = serverAddress;
}

QNetworkReply *HTTPClientBase::startFileUpload(const QString &localFilePath, const QString &fileTarget)
{
    std::shared_ptr<QFile> file = std::make_shared<QFile>(localFilePath);
    file->open(QIODevice::ReadOnly);
    if (!file->isOpen()) {
        throw std::invalid_argument("Download failed: Invalid file path");
    }

    auto request = createRequest(fileTarget);
    auto reply = m_requester.post(request, file.get());
    connect(reply, &QNetworkReply::finished, [reply, file]() { // For file correct existance during upload
        file->close();
    });
    return reply;
}

QNetworkReply *HTTPClientBase::startFileDownload(const QString &localSavefile, const QString &fileTarget)
{
    std::shared_ptr<QFile> file = std::make_shared<QFile>(localSavefile);
    if (!file->open(QIODevice::WriteOnly)) {
        throw std::invalid_argument("Download failed: Invalid file path");
    }

    auto request = createRequest(fileTarget);
    auto reply = m_requester.get(request);
    connect(reply, &QNetworkReply::readyRead, [reply, file]() {
        file->write(reply->readAll());
    });
    connect(reply, &QNetworkReply::finished, [reply, file]() { // For file correct existance during download
        file->close();
    });
    return reply;
}

void HTTPClientBase::setCommonHeader(const QString &headerName, const QString &headerData)
{
    m_commonHeaders[headerName.toUtf8()] = headerData.toUtf8();
}

QNetworkRequest HTTPClientBase::createRequest(const QString &target, const QStringList &args) const
{
    QNetworkRequest res;
    res.setUrl(HTTPCommon::createUrl(m_serverAddress, target, args));

    for (auto& [headerName, headerData] : m_commonHeaders) {
        res.setRawHeader(headerName, headerData);
    }

    return res;
}

QNetworkAccessManager &HTTPClientBase::getRequester()
{
    return m_requester;
}

}