#include "servermanagementform.hpp"
#include "ui_servermanagementform.h"

#include "serverlistmodel.hpp"
#include "common.hpp"

#include <Components/Logger/Logger.h>

#include <QNetworkRequest>
#include <QNetworkReply>

#include <QTimer>

#include <QJsonDocument>
#include <QJsonObject>

#include <QMessageBox>

ServerManagementForm::ServerManagementForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ServerManagementForm)
{
    ui->setupUi(this);

    m_pServerModel = new ServerListModel(this);
    ui->listViewServers->setModel(m_pServerModel);

    connect(ui->listViewServers->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, [this](auto& selectedIdx, auto& deselectedIdx){
        if (selectedIdx.isEmpty()) {
            stopUpdateTimer();
            return;
        }
        setServer(selectedIdx.indexes().front().data(Qt::DisplayRole).toString());
    });

    connect(ui->pushButtonShutdown, &QPushButton::clicked,
            this, [this](){
        QNetworkRequest req;
        req.setUrl(Common::createUrl(m_currentServerIp, "/api/server/power?action=poweroff"));
        m_requestManager.put(req, QByteArray());
    });

    connect(ui->pushButtonReboot, &QPushButton::clicked,
            this, [this](){
        QNetworkRequest req;
        req.setUrl(Common::createUrl(m_currentServerIp, "/api/server/power?action=reboot"));
        m_requestManager.put(req, QByteArray());
    });

    connect(&m_requestManager, &QNetworkAccessManager::finished,
            this, [this](QNetworkReply* pReply){
        if (pReply->error() == QNetworkReply::ConnectionRefusedError ||
            pReply->error() == QNetworkReply::RemoteHostClosedError ||
            pReply->error() == QNetworkReply::HostNotFoundError ||
            pReply->error() == QNetworkReply::TimeoutError) {
            stopUpdateTimer();
            QMessageBox::warning(this, "Внимание!", QString("Потеряно соединение с сервером:\n %0").arg(pReply->request().url().toString()));
            return;
        }

        auto target = Common::getTarget(pReply->request().url());
        if (target != "/api/server/status") {
            COMPLOG_DEBUG("INVALID TARGET:", target.toStdString());
            return;
        }

        auto jsonResponce = pReply->readAll();
        QJsonParseError err;
        auto obj = QJsonDocument::fromJson(jsonResponce, &err).object();
        if (obj.isEmpty()) {
            COMPLOG_ERROR("FAILED TO PROCESS STATUS RESPONCE:", err.errorString().toStdString());
            return;
        }

        // {"cpu_load":9.090909090909093,"cpu_temp":46.125,"space_available":137500889088,"space_free":163122790400,"space_total":502914768896,"uptime":106209}

        // Get uptime
        auto uptimeSecs = obj["uptime"].toInt();
        auto uptimeLeast = uptimeSecs % 86400;
        auto uptimeDays = uptimeSecs / 86400;
        ui->spinBoxUptime->setValue(uptimeDays);
        ui->timeEditUptime->setTime( QTime(uptimeLeast / 3600, uptimeLeast % 3600 / 60, uptimeLeast % 3600 % 60) );

        // Get temperature
        ui->spinBoxTempCurrent->setValue(obj["cpu_temp"].toDouble());

        // Get CPU load percent
        ui->spinBoxCPUCurrent->setValue(obj["cpu_load"].toDouble());

        // Get space
        auto totalSpace = obj["space_total"].toDouble() / 1024.0 / 1024.0 / 1024.0;
        auto freeSpace = obj["space_free"].toDouble() / 1024.0 / 1024.0 / 1024.0;
        ui->doubleSpinBoxSpaceCurrent->setValue(freeSpace);
        ui->doubleSpinBoxSpaceTotal->setValue(totalSpace);
    });
}

ServerManagementForm::~ServerManagementForm()
{
    delete ui;
}

void ServerManagementForm::addServer(const QString &serverName, const QString &serverHost)
{
    m_pServerModel->addServer(serverName, serverHost);
}

void ServerManagementForm::setServer(const QString &serverName)
{
    stopUpdateTimer();
    m_currentServerIp = m_pServerModel->getIp(serverName);
    COMPLOG_INFO("Selected server:", m_currentServerIp.toStdString());
    m_isUpdatesCalled = true;
    startUpdateTimer();
}

void ServerManagementForm::startUpdateTimer()
{
    QTimer::singleShot(m_timerUpdateTimeMs, [this](){
        if (!m_isUpdatesCalled) {
            return;
        }
        QNetworkRequest req;
        req.setUrl(Common::createUrl(m_currentServerIp, "/api/server/status"));
        m_requestManager.get(req);

        if (m_isUpdatesCalled) {
            startUpdateTimer();
        }
    });
}

void ServerManagementForm::stopUpdateTimer()
{
    m_isUpdatesCalled = false;
}

void ServerManagementForm::showEvent(QShowEvent *e)
{
    if (ui->listViewServers->currentIndex().isValid()) {
        startUpdateTimer();
    }
    QWidget::showEvent(e);
}

void ServerManagementForm::hideEvent(QHideEvent *e)
{
    stopUpdateTimer();
    QWidget::hideEvent(e);
}
