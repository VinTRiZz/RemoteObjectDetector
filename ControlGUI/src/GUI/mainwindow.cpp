#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_pStatusModel {new QStandardItemModel(this)},
    m_server {new ControlServer(this)},
    m_requestTimer{new QTimer(this)}
{
    ui->setupUi(this);

    qRegisterMetaType<ConnectedDevice>("ConnectedDevice");

    ui->menu_stackedWidget->setCurrentIndex(0);
    ui->devStatus_tableView->setModel(m_pStatusModel);
    ui->devStatus_tableView->setSizeAdjustPolicy(QTableView::AdjustToContents);
    ui->devStatus_tableView->verticalHeader()->hide();
    ui->devStatus_tableView->horizontalHeader()->setStretchLastSection(true);

    connect(ui->device_comboBox, &QComboBox::currentTextChanged, this, &MainWindow::setDevice);

    connect(m_requestTimer, &QTimer::timeout, this, &MainWindow::periodicRequest);

    // server connections
    connect(m_server, &ControlServer::errorGot, this, &MainWindow::addMessageToHistory);
    connect(m_server, &ControlServer::deviceConnected, this, &MainWindow::addConnection);
    connect(m_server, &ControlServer::deviceDisconnected, this, &MainWindow::removeConnection);
    connect(m_server, &ControlServer::deviceSetupComplete, this, &MainWindow::deviceIsReady);
    connect(m_server, &ControlServer::deviceStarted, this, &MainWindow::deviceStarted);
    connect(m_server, &ControlServer::deviceStopped, this, &MainWindow::deviceStopped);
    connect(m_server, &ControlServer::objectAdded, this, &MainWindow::objectAdded);
    connect(m_server, &ControlServer::objectRenamed, this, &MainWindow::objectRenamed);
    connect(m_server, &ControlServer::objectRemoved, this, &MainWindow::objectRemoved);
    connect(m_server, &ControlServer::deviceStatusGot, this, &MainWindow::deviceStatusGot);

//    startTestFunction();
    if (!m_server->init(9001))
    {
        ui->statusbar->setStatusTip("Internal error. Please restart application");
        return;
    }
    m_requestTimer->start(m_updateTime);
    emit addMessageToHistory("Init complete");
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_systemMenu_pushButton_clicked()
{
    ui->menu_stackedWidget->setCurrentIndex(0);
}


void MainWindow::on_typeManagerMenu_pushButton_clicked()
{
    ui->menu_stackedWidget->setCurrentIndex(1);
}


void MainWindow::on_monitor_pushButton_clicked()
{
    ui->menu_stackedWidget->setCurrentIndex(2);
}

void MainWindow::addConnection(const QString &devToken)
{
    m_deviceTokens.push_back(devToken);
    emit addMessageToHistory(QString("Connected with device %1").arg(devToken));
    updateDeviceList();
}

void MainWindow::removeConnection(const QString &devToken)
{
    emit addMessageToHistory(QString("Connection with %1 removed").arg(devToken));
    auto devTokenPos = std::find(m_deviceTokens.begin(), m_deviceTokens.end(), devToken);
    if (devTokenPos == m_deviceTokens.end())
        return;
    m_deviceTokens.erase(devTokenPos);
    if (devToken == m_currentDevice.token)
    {
        m_currentDevice.isValid = false;
        cleanDeviceContent();
    }
    updateDeviceList();
}

void MainWindow::deviceIsReady(const QString &devToken)
{
    m_currentDevice.canWork = true;
    emit addMessageToHistory(QString("Device %1 is ready").arg(devToken));
}

void MainWindow::deviceStarted(const QString &devToken)
{
    m_currentDevice.isWorking = true;
    emit addMessageToHistory(QString("Device %1 started").arg(devToken));
}

void MainWindow::deviceStopped(const QString &devToken)
{
    m_currentDevice.isWorking = false;
    emit addMessageToHistory(QString("Device %1 stopped").arg(devToken));
}

void MainWindow::objectAdded(const QString &objectName)
{
    ui->objects_listWidget->addItem(objectName);
    emit addMessageToHistory(QString("Added object %1").arg(objectName));
}

void MainWindow::objectRenamed(const QString &objectName, const QString &newName)
{
    auto itemList = ui->objects_listWidget->findItems(objectName, Qt::MatchExactly);
    if (itemList.isEmpty()) return;
    itemList[0]->setText(newName);
    emit addMessageToHistory(QString("Renamed object %1 to %2").arg(objectName, newName));
}

void MainWindow::objectRemoved(const QString &objectName)
{
    auto itemList = ui->objects_listWidget->findItems(objectName, Qt::MatchExactly);
    if (itemList.isEmpty()) return;
    ui->objects_listWidget->removeItemWidget(itemList[0]);
    emit addMessageToHistory(QString("Removed object %1").arg(objectName));
}

void MainWindow::deviceStatusGot(const Exchange::StatusData &devStatus)
{
    // Status setup
    m_pStatusModel->clear();
    m_pStatusModel->setColumnCount(2);
    for (auto& statusPair : devStatus.statusMap)
    {
        QList<QStandardItem*> items;
        items.push_back(new QStandardItem(statusPair.first.c_str()));
        items.push_back(new QStandardItem(statusPair.second.c_str()));
        m_pStatusModel->appendRow(items);
    }
}

void MainWindow::addMessageToHistory(const QString &messageText)
{
    qDebug() << "New message:" << messageText;
    ui->history_listWidget->addItem(messageText);
}

void MainWindow::periodicRequest()
{
    m_requestTimer->stop();
    if (m_currentDevice.isValid)
        m_server->status(m_currentDevice.token);

    if (m_currentDevice.cameraEnabled)
        m_server->photo(m_currentDevice.token);

    m_requestTimer->start(m_updateTime);
}

void MainWindow::updateDeviceList()
{
    m_requestTimer->stop();
    ui->device_comboBox->clear();
    for (auto& dev : m_deviceTokens)
    {
        ui->device_comboBox->addItem(dev);
    }
    m_requestTimer->start(m_updateTime);
    emit addMessageToHistory("Device list updated");
}

void MainWindow::setDevice(const QString &devToken)
{
    m_requestTimer->stop();
    if (devToken == "")
    {
        m_currentDevice.isValid = false;
        return;
    }

    // Clear from previous device
    cleanDeviceContent();

    // TODO: Load info from database (including name)
    m_currentDevice.name = devToken; // TODO: Remove
    m_currentDevice.token = devToken;

    // UI updates
    ui->name_lineEdit->setText(devToken);

    m_currentDevice.isValid = true;
    m_requestTimer->start(m_updateTime);
    emit addMessageToHistory("Device changed");
}

void MainWindow::cleanDeviceContent()
{
    ui->objects_listWidget->clear();
    ui->match_listWidget->clear();
    ui->name_lineEdit->clear();
    ui->camera_label->clear();
    m_pStatusModel->clear();
}

void MainWindow::startTestFunction()
{
    ConnectedDevice dev;
    dev.name = "Test dev";
    dev.objects.push_back("Pen");
    dev.objects.push_back("Pencil");
    dev.objects.push_back("Rubber");
    dev.objects.push_back("Aircraft");
    dev.objects.push_back("Cleaner");

    dev.cameraEnabled = false;
    dev.token = "891duk3qwhauknhbcvulwacbiwe";

    Exchange::StatusData devStatus;
    devStatus.statusMap["CPU load"] = "33 %";
    devStatus.statusMap["CPU temp."] = "65 C";
    devStatus.statusMap["Analyse interval"] = "1 s";
    devStatus.statusMap["Image send interval"] = "5 s";
    devStatus.statusMap["Template count"] = "30";
    devStatus.statusMap["Power on time"] = "02.02.2024";
    devStatus.statusMap["Position"] = "Somewhere in Moscow";

    m_currentDevice = dev;

    emit setDevice(dev.name);
}

