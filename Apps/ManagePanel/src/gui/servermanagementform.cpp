#include "servermanagementform.hpp"
#include "ui_servermanagementform.h"

#include "models/serverlistmodel.hpp"
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

        auto serverRow = selectedIdx.indexes().front().row();
        auto serverConf = m_pServerModel->getServer(serverRow)->get(); // Server must exist here
        auto serverAddr = serverConf.getAddress();
        m_pServerManager->setServer(QString("%0:%1").arg(serverAddr.first, serverAddr.second));
        COMPLOG_INFO("Selected server:", serverConf.getName().toStdString());

        m_isUpdatesCalled = true;
        startUpdateTimer();

        emit serverSelected(serverConf);
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

void ServerManagementForm::addServer(const ServerConfiguration& conf)
{
    m_pServerModel->addServer(conf);
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
