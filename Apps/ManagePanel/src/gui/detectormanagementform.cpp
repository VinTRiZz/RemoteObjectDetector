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
}

DetectorManagementForm::~DetectorManagementForm()
{
    delete ui;
}

void DetectorManagementForm::setServer(const QString &serverAddress)
{

}
