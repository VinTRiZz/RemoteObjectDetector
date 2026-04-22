#include "detectoradddialog.hpp"
#include "ui_detectoradddialog.h"

#include <QDateTime>

DetectorAddDialog::DetectorAddDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DetectorAddDialog)
{
    ui->setupUi(this);

    ui->configurationWidget->setCreateMode(false);

    DataObjects::DetectorConfiguration conf;
    conf.system.registerDateUTC = QDateTime::currentSecsSinceEpoch();
    ui->configurationWidget->showConfiguration(conf);
}

DetectorAddDialog::~DetectorAddDialog()
{
    delete ui;
}

DataObjects::DetectorConfiguration DetectorAddDialog::readConfiguration() const
{
    return ui->configurationWidget->readConfiguration();
}
