#include "detectormanagementform.hpp"
#include "ui_detectormanagementform.h"

#include "client/detectorinfomanager.hpp"

#include "models/detectorlistmodel.hpp"

DetectorManagementForm::DetectorManagementForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DetectorManagementForm)
{
    ui->setupUi(this);

    m_pDetectorInfoManager = new DetectorInfoManager(this);

    m_pDetectorListModel = new DetectorListModel(this);
    m_pDetectorListModel->setDetectorInfoManager(m_pDetectorInfoManager);

    ui->listViewDetectorList->setModel(m_pDetectorListModel);
    ui->listViewDetectorList->setModelColumn(DetectorListModel::C_name);
}

DetectorManagementForm::~DetectorManagementForm()
{
    delete ui;
}

void DetectorManagementForm::setServer(const ServerConfiguration &conf)
{
    auto addr = conf.getAddress();
    m_pDetectorInfoManager->setServer(QString("%0:%1").arg(addr.first, addr.second));
    m_pDetectorListModel->updateDetectorList();
}
