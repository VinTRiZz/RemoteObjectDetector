#pragma once

#include <QObject>
#include <QNetworkAccessManager>

#include <map>

namespace Web::Implementation
{

class HTTPClientBase : public QObject
{
    Q_OBJECT
public:
    explicit HTTPClientBase(QObject *parent = nullptr);

    /**
     * @brief setServer     Set address of a server to send requests
     * @param serverAddress Already checked, correct server address if format ip:port
     */
    void setServer(const QString& serverAddress);

    /**
     * @brief startFileUpload   Start file upload as multipart HTTP item
     * @param localFilePath     File to send (absolute or relative)
     * @param fileTarget        Target on server to send POST request
     * @return                  Reply object, handling file upload
     * @throws                  std::invalid_argument exception if file is invalid
     */
    QNetworkReply* startFileUpload(const QString& localFilePath, const QString& fileTarget);

    /**
     * @brief startFileDownload Start file download as multipart HTTP item
     * @param saveDirectory     Local file path to save as (absolute or relative)
     * @param fileTarget        Target on server to send GET request
     * @return                  Reply object, handling file download
     * @throws                  std::invalid_argument exception if directory is invalid
     */
    QNetworkReply* startFileDownload(const QString& localSavefile, const QString& fileTarget);

private:
    QString                             m_serverAddress;
    QNetworkAccessManager               m_requester;
    std::map<QByteArray, QByteArray>    m_commonHeaders;

protected:
    /**
     * @brief setCommonHeader   Adds header for createRequest generator. Does not check value correctness
     * @param headerName        Header name, in UTF-8, must be correct header
     * @param headerData        Header data, in UTF-8, must be correct value
     */
    void setCommonHeader(const QString& headerName, const QString& headerData);


    /**
     * @brief createRequest Create request object using added common headers, target and parameters. Does not check value correctness
     * @param target        Path, for example /api/v2/status
     * @param args          Args of target, in format "NAME=VALUE"
     * @return              Generated object, ready for using
     */
    QNetworkRequest createRequest(const QString& target, const QStringList& args = {}) const;
    QNetworkAccessManager& getRequester();
};

}