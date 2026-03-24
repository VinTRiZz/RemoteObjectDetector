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
    connect(ui->pushButtonShutdown, &QPushButton::clicked,
            m_pServerManager, &ServerManager::requestShutdown);
    connect(ui->pushButtonReboot, &QPushButton::clicked,
            m_pServerManager, &ServerManager::requestReboot);

    // Status
    connect(m_pServerManager, &ServerManager::responseStatus,
            this, [this](bool isOk, const Protocol::Structures::DeviceStatus& serverStatus){
        // Uptime
        auto uptimeLeast = serverStatus.common.uptime % 86400;
        auto uptimeDays = serverStatus.common.uptime / 86400;
        ui->spinBoxUptime->setValue(uptimeDays);
        ui->timeEditUptime->setTime( QTime(uptimeLeast / 3600, uptimeLeast % 3600 / 60, uptimeLeast % 3600 % 60) );

        // CPU info
        ui->spinBoxTempCurrent->setValue(serverStatus.cpu.temperature);
        ui->spinBoxCPUCurrent->setValue(serverStatus.cpu.loadPercent);

        // Storage info
        auto totalSpace = serverStatus.storage.spaceTotal / 1024.0 / 1024.0 / 1024.0;
        auto freeSpace = serverStatus.storage.spaceFree / 1024.0 / 1024.0 / 1024.0;
        ui->doubleSpinBoxSpaceCurrent->setValue(freeSpace);
        ui->doubleSpinBoxSpaceTotal->setValue(totalSpace);
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
