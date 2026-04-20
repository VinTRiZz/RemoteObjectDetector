#include "servermanagementform.hpp"
#include "ui_servermanagementform.h"

#include <Components/Logger/Logger.h>

#include <QIdentityProxyModel>
#include <QMessageBox>

#include "gui/models/servertreemodel.hpp"
#include "gui/dialogs/serveradddialog.hpp"

#include "client/serverregistry.hpp"
#include "client/server.hpp"

ServerManagementForm::ServerManagementForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ServerManagementForm)
{
    ui->setupUi(this);

    m_pServerTreeIdentityModel = new QIdentityProxyModel(this);
    ui->treeViewServers->setModel(m_pServerTreeIdentityModel);
    ui->treeViewServers->header()->setSectionResizeMode(QHeaderView::Stretch);

    connect(ui->treeViewServers->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, [this](auto& selectedIdx, auto& deselectedIdx){
        if (selectedIdx.isEmpty()) {
            processSelectedServer({});
            return;
        }

        auto serverRow = selectedIdx.indexes().front().row();
        auto serverConf = m_pServerTreeModel->getServer(serverRow);
        processSelectedServer(serverConf);
    });
    connect(ui->pushButtonAddServer, &QPushButton::clicked,
            this, [this](){
        if (!m_pCurrentServerRegistry) {
            return;
        }
        ServerAddDialog dial;
        auto execRes = dial.exec();
        if (execRes == QDialog::Accepted && !m_pCurrentServerRegistry->addServer(dial.getConfig())) {
            QMessageBox::critical(this, "Server add error", m_pCurrentServerRegistry->getLastErrorText());
        }
    });
    ui->pushButtonAddServer->setEnabled(false);

    connect(ui->pushButtonRemoveServer, &QPushButton::clicked,
            this, [this](){
        if (!m_pCurrentServerRegistry) {
            return;
        }
        auto targetServer = m_pServerTreeModel->getServer(ui->treeViewServers->currentIndex().row());
        if (!targetServer.isValid()) {
            return;
        }
        auto& conf = targetServer->getConfiguration();

        auto userResp = QMessageBox::question(
            this,
            "Removing server",
            QString("Are you sure removing this server?\n%0 (%1:%2)").arg(conf.getName(), conf.getHost(), QString::number(conf.getPort())),
            QMessageBox::StandardButton::Yes, QMessageBox::StandardButton::No);
        if (userResp == QMessageBox::StandardButton::Yes) {
            m_pCurrentServerRegistry->removeServer(conf);
        }
    });
}

ServerManagementForm::~ServerManagementForm()
{
    delete ui;
}

void ServerManagementForm::setSourceServerModel(ServerTreeModel *pSourceModel)
{
    m_pServerTreeIdentityModel->setSourceModel(pSourceModel);
    m_pServerTreeModel = pSourceModel;

    if (m_pCurrentServerRegistry) {
        // TODO: Disconnect everything
    }

    m_pCurrentServerRegistry = pSourceModel->getServerRegistry();

    ui->pushButtonAddServer->setEnabled(false);
    if (m_pCurrentServerRegistry) {
        // TODO: Connect everything
        ui->pushButtonAddServer->setEnabled(true);
    }
    processSelectedServer({});
}

void ServerManagementForm::processSelectedServer(const Web::ServerHandler &serv)
{
    ui->pushButtonRemoveServer->setEnabled(serv.isValid());
    if (!serv.isValid()) {
        ui->serverStatus->setStatus({});
        return;
    }

    // Power options
    connect(ui->serverStatus, &SystemStatusWidget::requestedPoweroff,
            serv.get(), &Web::Server::requestPoweroff);
    connect(ui->serverStatus, &SystemStatusWidget::requestedReboot,
            serv.get(), &Web::Server::requestReboot);

    // Status
    connect(ui->serverStatus, &SystemStatusWidget::requestUpdateInfo,
            serv.get(), &Web::Server::requestStatus);
    connect(serv.get(), &Web::Server::gotStatus,
            ui->serverStatus, &SystemStatusWidget::setStatus);
}
