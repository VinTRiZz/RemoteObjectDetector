#pragma once

#include <QWidget>

#include <QNetworkAccessManager>
#include <QNetworkRequest>

class ServerListModel;
class ServerManager;

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

signals:
    void serverSelected(const QString& serverAddress);

private:
    Ui::ServerManagementForm *ui;

    // Network
    ServerManager*   m_pServerManager {nullptr};

    // GUI
    ServerListModel* m_pServerModel {nullptr};

    // Status update thing
    unsigned    m_timerUpdateTimeMs {1000};
    bool        m_isUpdatesCalled {false};

    void startUpdateTimer();
    void stopUpdateTimer();

protected:
    void showEvent(QShowEvent* e) override;
    void hideEvent(QHideEvent* e) override;
};

