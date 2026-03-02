#include "detectormanagementform.hpp"
#include "ui_detectormanagementform.h"

DetectorManagementForm::DetectorManagementForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DetectorManagementForm)
{
    ui->setupUi(this);
}

DetectorManagementForm::~DetectorManagementForm()
{
    delete ui;
}
