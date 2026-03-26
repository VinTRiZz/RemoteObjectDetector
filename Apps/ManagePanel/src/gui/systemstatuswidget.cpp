#include "systemstatuswidget.hpp"
#include "ui_systemstatuswidget.h"

#include <QTime>

SystemStatusWidget::SystemStatusWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SystemStatusWidget)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);

    connect(ui->pushButtonPoweroff, &QPushButton::clicked,
            this, &SystemStatusWidget::requestedPoweroff);
    connect(ui->pushButtonReboot, &QPushButton::clicked,
            this, &SystemStatusWidget::requestedReboot);
}

SystemStatusWidget::~SystemStatusWidget()
{
    delete ui;
}

void SystemStatusWidget::setInvalidState()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void SystemStatusWidget::setDisplayInfo(const DataObjects::DeviceStatus &status)
{
    ui->stackedWidget->setCurrentIndex(1);
    if (!status.isValid()) {
        setInvalidState();
        return;
    }

    // Uptime
    auto uptimeLeast = status.common.uptime % 86400;
    auto uptimeDays = status.common.uptime / 86400;
    auto uptimeObj = QTime(uptimeLeast / 3600, uptimeLeast % 3600 / 60, uptimeLeast % 3600 % 60);
    ui->lineEditUptime->setText(QString("%1 d %2 h %3 m %4 s").arg(
        QString::number(uptimeDays),
        QString::number(uptimeLeast / 3600),
        QString::number(uptimeLeast % 3600 / 60),
        QString::number(uptimeLeast % 3600 % 60)));

    // CPU info
    ui->spinBoxTempCurrent->setValue(status.cpu.temperature);
    ui->spinBoxCPUCurrent->setValue(status.cpu.loadPercent);

    // Storage info
    auto totalSpace = status.storage.spaceTotal / 1024.0 / 1024.0 / 1024.0;
    auto freeSpace = status.storage.spaceFree / 1024.0 / 1024.0 / 1024.0;
    ui->doubleSpinBoxSpaceFree->setValue(freeSpace);
    ui->doubleSpinBoxSpaceTotal->setValue(totalSpace);
}
