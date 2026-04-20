#include "servermanagementform.hpp"
#include "ui_servermanagementform.h"

#include <Components/Logger/Logger.h>

#include <QIdentityProxyModel>
#include <QMessageBox>

#include "gui/models/servertreemodel.hpp"
#include "client/serverregistry.hpp"
#include "client/server.hpp"

ServerManagementForm::ServerManagementForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ServerManagementForm)
{
    ui->setupUi(this);

    m_pServerTreeIdentityModel = new QIdentityProxyModel(this);
    ui->treeViewServers->setModel(m_pServerTreeIdentityModel);

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
}

ServerManagementForm::~ServerManagementForm()
{
    delete ui;
}

void ServerManagementForm::setSourceServerModel(ServerTreeModel *pSourceModel)
{
    m_pServerTreeIdentityModel->setSourceModel(pSourceModel);

    if (m_pCurrentServerRegistry) {
        // TODO: Disconnect everything
    }
    m_pCurrentServerRegistry = pSourceModel->getServerRegistry();
    if (m_pCurrentServerRegistry) {
        // TODO: Connect everything
    }
}

void ServerManagementForm::processSelectedServer(const Web::ServerHandler &serv)
{
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
