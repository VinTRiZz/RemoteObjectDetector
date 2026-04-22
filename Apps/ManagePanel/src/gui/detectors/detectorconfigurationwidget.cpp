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

void DetectorConfigurationWidget::setCreateMode(bool isCreateMode)
{
    ui->groupBoxOnline->setVisible(isCreateMode);
    ui->comboBoxSoftwareVersion->setVisible(isCreateMode);
}

void DetectorConfigurationWidget::showConfiguration(const DataObjects::DetectorConfiguration &conf)
{
    m_conf = conf;

    // Info
    ui->lineEditName->setText(m_conf.info.name->c_str());
    ui->lineEditLocation->setText(m_conf.info.location->c_str());
    ui->plainTextEditDescription->setPlainText(m_conf.info.description->c_str());

    // Times
    ui->dateTimeEditRegisterDate->setDateTime(QDateTime::fromTime_t(m_conf.system.registerDateUTC));
    ui->dateTimeEditLastOnline->setDateTime(QDateTime::fromTime_t(m_conf.online.lastOnlineTimeUTC));

    // TODO: Handle version
}

DataObjects::DetectorConfiguration DetectorConfigurationWidget::readConfiguration()
{
    // Info
    m_conf.info.name = ui->lineEditName->text().toStdString();
    m_conf.info.location = ui->lineEditLocation->text().toStdString();
    m_conf.info.description = ui->plainTextEditDescription->toPlainText().toStdString();

    // Times
    m_conf.system.registerDateUTC = ui->dateTimeEditRegisterDate->dateTime().toTime_t();
    m_conf.online.lastOnlineTimeUTC = ui->dateTimeEditLastOnline->dateTime().toTime_t();

    // TODO: Handle version
    m_conf.software.versionId = 1; // DEBUG
    m_conf.software.updateTimeUTC = 0;

    return m_conf;
}
