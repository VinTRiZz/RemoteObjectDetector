#include "systemstatuswidget.hpp"
#include "ui_systemstatuswidget.h"

#include <QTime>
#include <QTimer>

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

void SystemStatusWidget::setStatus(const DataObjects::DeviceStatus &status)
{
    if (!status.isValid()) {
        ui->stackedWidget->setCurrentIndex(0);
        return;
    }
    ui->stackedWidget->setCurrentIndex(1);

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

    // Visual data optimisation
    if (this->isVisible()) {
        restartUpdateTimer();
    }
}

void SystemStatusWidget::restartUpdateTimer()
{
    // TODO: Could be better somehow? If required
    QTimer::singleShot(m_timerUpdateTimeMs,
                       this, &SystemStatusWidget::requestUpdateInfo);
}

void SystemStatusWidget::showEvent(QShowEvent *e)
{
    if (ui->stackedWidget->currentIndex() != 0) { // 0 is placeholder
        restartUpdateTimer();
    }
    QWidget::showEvent(e);
}
