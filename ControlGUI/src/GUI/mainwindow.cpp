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

    connect(m_server, &ControlServer::objectListGot, this, &MainWindow::objectListGot);
    connect(m_server, &ControlServer::objectDetectedListGot, this, &MainWindow::objectDetectedListGot);
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

void MainWindow::objectListGot(const QString &objectList)
{
    // TODO: Parse
}

void MainWindow::objectDetectedListGot(const QString &objectDetectedList)
{
    // TODO: Parse
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

    // Add position from database
    auto positionString = "Somewhere"; // TODO: Load from database
    QList<QStandardItem*> items;
    items.push_back(new QStandardItem("Position"));
    items.push_back(new QStandardItem(positionString));
    m_pStatusModel->appendRow(items);

    // Add update period
    items.clear();
    items.push_back(new QStandardItem("Update period"));
    items.push_back(new QStandardItem(QString("%1 s").arg(QString::number(m_updateTime / 1000))));
    m_pStatusModel->appendRow(items);

    // Camera is enabled
    items.clear();
    items.push_back(new QStandardItem("Camera enabled"));
    items.push_back(new QStandardItem(m_currentDevice.cameraEnabled ? "Yes" : "No"));
    m_pStatusModel->appendRow(items);
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

    m_server->getObjectList(m_currentDevice.token);

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


void MainWindow::on_init_pushButton_clicked()
{
    m_server->setup(m_currentDevice.token);
    emit addMessageToHistory("Device init called");
}


void MainWindow::on_viewEnable_pushButton_clicked()
{
    m_currentDevice.cameraEnabled = true;
}


void MainWindow::on_viewDisable_pushButton_clicked()
{
    m_currentDevice.cameraEnabled = false;
}


void MainWindow::on_start_pushButton_clicked()
{
    m_server->start(m_currentDevice.token);
}


void MainWindow::on_stop_pushButton_clicked()
{
    m_server->stop(m_currentDevice.token);
}


void MainWindow::on_reboot_pushButton_clicked()
{
    m_server->reboot(m_currentDevice.token);
}


void MainWindow::on_addObject_pushButton_clicked()
{
    auto objectName = ui->objectName_lineEdit->text();
    if (!objectName.size())
    {
        emit addMessageToHistory("Error: no name written");
        return;
    }
    m_server->addObject(objectName, m_currentDevice.token);
}


void MainWindow::on_objects_listWidget_itemClicked(QListWidgetItem *item)
{
    ui->objectName_lineEdit->setText(item->text());
}


void MainWindow::on_removeObject_pushButton_clicked()
{
    auto pItem = ui->objects_listWidget->currentItem();
    if (!pItem)
    {
        emit addMessageToHistory("Error: no object selected");
        return;
    }
    m_server->removeObject(pItem->text(), m_currentDevice.token);
}


void MainWindow::on_renameObject_pushButton_clicked()
{
    auto pItem = ui->objects_listWidget->currentItem();
    if (!pItem)
    {
        emit addMessageToHistory("Error: no object selected");
        return;
    }
    auto newName = ui->objectName_lineEdit->text();
    if (!newName.size())
    {
        emit addMessageToHistory("Error: no name written");
        return;
    }
    m_server->renameObject(pItem->text(), newName, m_currentDevice.token);
}


void MainWindow::on_DEBUG_pushButton_clicked()
{
    m_server->setup(m_currentDevice.token);
    m_server->start(m_currentDevice.token);
}

