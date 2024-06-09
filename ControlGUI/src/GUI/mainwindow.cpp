#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_pStatusModel {new QStandardItemModel(this)},
    m_server {new ControlServer(this)}
{
    ui->setupUi(this);

    qRegisterMetaType<ConnectedDevice>("ConnectedDevice");

    ui->menu_stackedWidget->setCurrentIndex(0);
    ui->devStatus_tableView->setModel(m_pStatusModel);
    ui->devStatus_tableView->setSizeAdjustPolicy(QTableView::AdjustToContents);
    ui->devStatus_tableView->verticalHeader()->hide();
    ui->devStatus_tableView->horizontalHeader()->setStretchLastSection(true);

    connect(ui->device_comboBox, &QComboBox::currentTextChanged, this, &MainWindow::setDevice);

    // server connections
    connect(m_server, &ControlServer::errorGot, this, &MainWindow::addMessageToHistory);
    connect(m_server, &ControlServer::deviceConnected, this, &MainWindow::addConnection);

//    startTestFunction();
    if (!m_server->init(9001))
    {
        ui->statusbar->setStatusTip("Internal error. Please restart application");
    }
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
    ConnectedDevice dev;
    dev.token = devToken;
    m_devices.push_back(dev);
    updateDeviceList();
}

void MainWindow::removeConnection(const QString devToken)
{
    m_devices.erase(std::find_if(m_devices.begin(), m_devices.end(), [&devToken](auto& dev){ return (dev.token == devToken); }));
}

void MainWindow::addMessageToHistory(const QString &messageText)
{
    qDebug() << "New message:" << messageText;
    ui->history_listWidget->addItem(messageText);
}

void MainWindow::updateDeviceList()
{
    qDebug() << "Updating devices info";
    for (auto& dev : m_devices)
    {
        ui->device_comboBox->addItem(dev.name);
    }
}

void MainWindow::setDevice(const QString &devName)
{
    // Clear from previous device
    m_pStatusModel->clear();
    ui->history_listWidget->clear();
    ui->objects_listWidget->clear();
    ui->match_listWidget->clear();
    ui->name_lineEdit->clear();
    ui->camera_label->clear();

    auto devDescriptorIt = std::find_if(m_devices.begin(), m_devices.end(), [&devName](auto& dev){ return (dev.name == devName); });
    if (devDescriptorIt == m_devices.end())
    {
        updateDeviceList();
        return;
    }

    auto devDescriptor = *devDescriptorIt;
    ui->name_lineEdit->setText(devName);

    m_pStatusModel->setColumnCount(2);
    for (auto& statusPair : devDescriptor.statusMap)
    {
        QList<QStandardItem*> items;
        items.push_back(new QStandardItem(statusPair.first.c_str()));
        items.push_back(new QStandardItem(statusPair.second.c_str()));
        m_pStatusModel->appendRow(items);
    }

    // TODO: Load info from database
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

    dev.statusMap["CPU load"] = "33 %";
    dev.statusMap["CPU temp."] = "65 C";
    dev.statusMap["Analyse interval"] = "1 s";
    dev.statusMap["Image send interval"] = "5 s";
    dev.statusMap["Template count"] = "30";
    dev.statusMap["Power on time"] = "02.02.2024";
    dev.statusMap["Position"] = "Somewhere in Moscow";

    m_devices.push_back(dev);

    emit setDevice(dev.name);
}

