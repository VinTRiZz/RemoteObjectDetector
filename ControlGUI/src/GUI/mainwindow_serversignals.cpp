#include "mainwindow.h"
#include "ui_mainwindow.h"


void MainWindow::addConnection(const QString &devToken)
{
    emit addMessageToHistory(QString("Connected with device %1").arg(devToken));

    if (getDeviceName(devToken).isEmpty())
    {
        ConnectedDevice dev;
        dev.token = devToken;
        uploadDeviceToDatabase(dev);
        m_deviceTokens.push_back(devToken);
    }

    updateDeviceList();
}


void MainWindow::removeConnection(const QString &devToken)
{
    m_currentDevice.isValid = false;
    cleanDeviceContent();
    m_currentDevice.isConnected = false;
    emit addMessageToHistory(QString("Connection with %1 removed").arg(devToken));
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
    qDebug() << "Got types:" << objectList;
}


void MainWindow::objectDetectedListGot(const QString &objectDetectedList)
{
    qDebug() << "Got object detected list:" << objectDetectedList;

    nlohmann::json objectsPacketJson;
    try {
        objectsPacketJson = nlohmann::json::parse(objectDetectedList.toStdString());
    } catch (nlohmann::json::exception& ex)
    {
        emit addMessageToHistory("Error parsing detected object list");
        return;
    }
    for (auto object = objectsPacketJson.begin(); object != objectsPacketJson.end(); object++ )
    {
        ui->match_listWidget->addItem(QString("%1  :  %2").arg(object.key().c_str(), object.value().dump().c_str()));
    }
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
    QList<QStandardItem*> items;
    items.push_back(new QStandardItem("Position"));
    items.push_back(new QStandardItem(m_currentDevice.place));
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
