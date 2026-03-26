#include "servermanagementform.hpp"
#include "ui_servermanagementform.h"

#include "serverlistmodel.hpp"
#include "client/servermanager.hpp"

#include <Components/Logger/Logger.h>

#include <QNetworkRequest>
#include <QNetworkReply>

#include <QTimer>

#include <QJsonDocument>
#include <QJsonObject>

#include <QMessageBox>

ServerManagementForm::ServerManagementForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ServerManagementForm)
{
    ui->setupUi(this);

    m_pServerManager = new ServerManager(this);

    m_pServerModel = new ServerListModel(this);
    ui->listViewServers->setModel(m_pServerModel);

    connect(ui->listViewServers->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, [this](auto& selectedIdx, auto& deselectedIdx){
        if (selectedIdx.isEmpty()) {
            stopUpdateTimer();
            return;
        }
        stopUpdateTimer();
        auto serverAddress = selectedIdx.indexes().front().data(ServerListModel::ServerAddress).toString();
        COMPLOG_INFO("Selected server:", serverAddress.toStdString());
        m_pServerManager->setServer(serverAddress);
        m_isUpdatesCalled = true;
        startUpdateTimer();
    });

    // Power options
    connect(ui->serverStatus, &SystemStatusWidget::requestedPoweroff,
            m_pServerManager, &ServerManager::requestShutdown);
    connect(ui->serverStatus, &SystemStatusWidget::requestedReboot,
            m_pServerManager, &ServerManager::requestReboot);

    // Status
    connect(m_pServerManager, &ServerManager::responseStatus,
            this, [this](bool isOk, const DataObjects::DeviceStatus& serverStatus){
        if (!isOk) {
            ui->serverStatus->setInvalidState();
            return;
        }
        ui->serverStatus->setDisplayInfo(serverStatus);
    });
}

ServerManagementForm::~ServerManagementForm()
{
    delete ui;
}

void ServerManagementForm::addServer(const QString &serverName, const QString &serverHost)
{
    m_pServerModel->addServer(serverName, serverHost);
}

void ServerManagementForm::startUpdateTimer()
{
    // TODO: Could be better somehow?
    QTimer::singleShot(m_timerUpdateTimeMs, [this](){
        if (!m_isUpdatesCalled) {
            return;
        }
        m_pServerManager->requestStatus();
        if (m_isUpdatesCalled) {
            startUpdateTimer();
        }
    });
}

void ServerManagementForm::stopUpdateTimer()
{
    m_isUpdatesCalled = false;
}

void ServerManagementForm::showEvent(QShowEvent *e)
{
    if (ui->listViewServers->currentIndex().isValid()) {
        startUpdateTimer();
    }
    QWidget::showEvent(e);
}

void ServerManagementForm::hideEvent(QHideEvent *e)
{
    stopUpdateTimer();
    QWidget::hideEvent(e);
}
