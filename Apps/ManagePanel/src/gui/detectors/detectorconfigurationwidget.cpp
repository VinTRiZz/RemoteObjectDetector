#include "detectorconfigurationwidget.hpp"
#include "ui_detectorconfigurationwidget.h"

DetectorConfigurationWidget::DetectorConfigurationWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DetectorConfigurationWidget)
{
    ui->setupUi(this);
}

DetectorConfigurationWidget::~DetectorConfigurationWidget()
{
    delete ui;
}

void DetectorConfigurationWidget::showConfiguration(const DataObjects::DetectorConfiguration &conf)
{
    ui->lineEditName->setText(conf.info.name.c_str());
    ui->lineEditLocation->setText(conf.info.location.c_str());
    ui->plainTextEditDescription->setPlainText(conf.info.description.c_str());

    ui->dateTimeEditRegisterDate->setDateTime(QDateTime::fromTime_t(conf.system.registerDateUTC));
    ui->dateTimeEditLastOnline->setDateTime(QDateTime::fromTime_t(conf.online.lastOnlineTimeUTC));
}

DataObjects::DetectorConfiguration DetectorConfigurationWidget::readConfiguration() const
{
    DataObjects::DetectorConfiguration res;

    res.info.name = ui->lineEditName->text().toStdString();
    res.info.location = ui->lineEditLocation->text().toStdString();
    res.info.description = ui->plainTextEditDescription->toPlainText().toStdString();

    res.system.registerDateUTC = ui->dateTimeEditRegisterDate->dateTime().toTime_t();

    res.online.lastOnlineTimeUTC = ui->dateTimeEditLastOnline->dateTime().toTime_t();

    return res;
}
