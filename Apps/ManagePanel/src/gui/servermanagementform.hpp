#pragma once

#include <QWidget>

#include <QNetworkAccessManager>
#include <QNetworkRequest>

class ServerListModel;

namespace Ui {
class ServerManagementForm;
}

class ServerManagementForm : public QWidget
{
    Q_OBJECT

public:
    explicit ServerManagementForm(QWidget *parent = nullptr);
    ~ServerManagementForm();

    void addServer(const QString& serverName, const QString& serverHost);

private slots:
    void setServer(const QString& serverName);

private:
    Ui::ServerManagementForm *ui;

    QNetworkAccessManager m_requestManager;

    ServerListModel* m_pServerModel {nullptr};
    QString m_currentServerIp;

    unsigned    m_timerUpdateTimeMs {1000};
    bool        m_isUpdatesCalled {false};

    void startUpdateTimer();
    void stopUpdateTimer();

protected:
    void showEvent(QShowEvent* e) override;
    void hideEvent(QHideEvent* e) override;
};

