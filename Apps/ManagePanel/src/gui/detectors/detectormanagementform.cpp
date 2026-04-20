#include "detectormanagementform.hpp"
#include "ui_detectormanagementform.h"

#include "client/server.hpp"
#include "common/serverconfiguration.hpp"

DetectorManagementForm::DetectorManagementForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DetectorManagementForm)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0); // Placeholder
}

DetectorManagementForm::~DetectorManagementForm()
{
    delete ui;
}

void DetectorManagementForm::setServer(const Web::ServerHandler &hdl)
{
    m_server = hdl;
    QString serverNameString;
    if (m_server.isValid()) {
        serverNameString = m_server.isValid() ? m_server->getConfiguration().getName() : "Invalid server";
    }
    setWindowTitle(QString("Detector management (%0)").arg(serverNameString));
}
