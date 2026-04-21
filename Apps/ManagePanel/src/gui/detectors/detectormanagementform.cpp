#include "detectormanagementform.hpp"
#include "ui_detectormanagementform.h"

#include "client/server.hpp"
#include "client/detector.hpp"
#include "client/detectorserver.hpp"
#include "common/serverconfiguration.hpp"
#include "detectoradddialog.hpp"

#include "gui/models/detectortreemodel.hpp"

#include <Components/Logger/Logger.h>

#include <QMessageBox>

DetectorManagementForm::DetectorManagementForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DetectorManagementForm)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0); // Placeholder

    m_pDetectorTreeModel = new DetectorTreeModel(this);
    ui->treeViewDetectors->setModel(m_pDetectorTreeModel);

    connect(ui->treeViewDetectors->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, [this](const QItemSelection &selected, const QItemSelection &deselected){
                if (selected.empty()) {
                    ui->stackedWidget->setCurrentIndex(0);
                    return;
                }
                ui->stackedWidget->setCurrentIndex(1);
                auto pDet = m_pDetectorTreeModel->getDetector(selected.indexes().front());
                ui->detectorHandleForm->showConfiguration(pDet.isValid() ? pDet->getConfiguration() : DataObjects::DetectorConfiguration{});
            });

    connect(ui->pushButtonSaveDetectorInfo, &QPushButton::clicked,
            this, [this](){
        auto selIndex = ui->treeViewDetectors->currentIndex();
        if (!selIndex.isValid()) {
            return;
        }
        auto pDet = m_pDetectorTreeModel->getDetector(selIndex);
        if (!pDet.isValid()) {
            QMessageBox::warning(this, "Detector data error", "Detector invalidated, failed to save changes");
            return;
        }
        auto conf = ui->detectorHandleForm->readConfiguration();
        conf.system.id = pDet->getConfiguration().system.id;
        pDet->setConfiguration(conf);
    });

    connect(ui->pushButtonAddDetector, &QPushButton::clicked,
            this, [this](){
        if (!m_server) {
            return;
        }
        DetectorAddDialog dial;
        auto execRes = dial.exec();
        auto pDetServer = m_server.cast<Web::DetectorServer>();
        if (execRes == QDialog::Accepted && !pDetServer->addDetector(dial.readConfiguration())) {
            QMessageBox::critical(this, "Detector add error", pDetServer->getLastErrorText());
        }
    });
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

    m_pDetectorTreeModel->setServer(m_server);
}
