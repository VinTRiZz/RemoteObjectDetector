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
    ui->camera_label->hide();

    connect(ui->device_comboBox, &QComboBox::currentTextChanged, this, &MainWindow::setDevice);

    connect(m_requestTimer, &QTimer::timeout, this, &MainWindow::periodicRequest);

    // server connections
    connect(m_server, &ControlServer::errorGot, this, &MainWindow::addMessageToHistory);
    connect(m_server, &ControlServer::photoGot, this, &MainWindow::photoGot);

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

#warning "Move path as a define"
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("cgui.db");
    if (!m_db.open())
    {
        emit addMessageToHistory("Warning: Error opening database. Setup database and reopen the app.");
        qDebug() << "Error text:" << m_db.lastError().text();
    }

    m_query = QSqlQuery(m_db);
    if (!setupDatabase())
    {
        emit addMessageToHistory("Error setting up database. Setup database and reopen the app.");
        qDebug() << "DB Setup error text:" << m_query.lastError().text();
        return;
    }
    m_db.close();
    m_db.open();
    loadAllTokens();

    m_server->setImageSize(400, 400);
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
    m_db.close();
    delete ui;
}

void MainWindow::addMessageToHistory(const QString &messageText)
{
    qDebug() << "New message:" << messageText;
    ui->statusbar->showMessage(messageText, (float)messageText.size() * 0.05f);
    ui->history_listWidget->addItem(messageText);
}

void MainWindow::periodicRequest()
{
    m_requestTimer->stop();
    ui->connectionStatus_label->setText( m_currentDevice.isConnected ? "Connected" : "N/a" );
    if (!m_currentDevice.isConnected)
        return;

    if (m_currentDevice.cameraEnabled && !m_imageIsLoadingNow)
    {
        m_server->photo(m_currentDevice.token);
        m_imageIsLoadingNow = true;
    }
    m_server->status(m_currentDevice.token);
    m_requestTimer->start(m_updateTime);
}

void MainWindow::updateDeviceList()
{
    m_requestTimer->stop();
    ui->device_comboBox->clear();
    for (auto& dev : m_deviceTokens)
    {
        ui->device_comboBox->addItem(getDeviceName(dev));
    }
    if (m_deviceTokens.size() == 1) setDevice(getDeviceName(m_deviceTokens.front()));
    m_requestTimer->start(m_updateTime);
    emit addMessageToHistory("Device list updated");
}

void MainWindow::setDevice(const QString &devName)
{
    if (m_currentDevice.isConnected) m_requestTimer->stop();
    if (devName == "")
    {
        m_currentDevice.isValid = false;
        return;
    }

    // Clear from previous device
    cleanDeviceContent();

    // Load device info
    if (m_currentDevice.isValid) updateDeviceInDatabase(m_currentDevice);
    m_currentDevice = loadDeviceFromDatabase(getDeviceToken(devName));
    m_currentDevice.isConnected = m_server->isConnected(m_currentDevice.token);

    // UI updates
    ui->name_lineEdit->setText(devName);

    m_imageIsLoadingNow = false;
    m_currentDevice.isValid = true;
    if (m_currentDevice.isConnected) m_requestTimer->start(m_updateTime);
    emit addMessageToHistory("Device changed");


    startFalsific();
}

void MainWindow::cleanDeviceContent()
{
    ui->objects_listWidget->clear();
    ui->match_listWidget->clear();
    ui->name_lineEdit->clear();
    m_pStatusModel->clear();
}

void MainWindow::startFalsific()
{
    std::srand(std::time(NULL));

//    QImage fimg("back.jpg", "jpg");
    QImage fimg("target.jpg", "jpg");
//    QImage fimg("object.jpg", "jpg");
    ui->camera_label->setPixmap(QPixmap::fromImage(fimg.scaled(QSize(300, 300))));
//    ui->camera_label->setPixmap(QPixmap::fromImage(fimg.scaled(QSize(400, 100))));
    ui->camera_label->show();


    std::vector<std::string> matchTypes {
        "Pen",
        "Pencil",
        "Flash",
        "Rubber",
        "Coin"
    };

    ui->match_listWidget->clear();
    for (int i = 0; i < matchTypes.size(); i++)
    {
        auto itemText = QString::fromStdString(matchTypes[i] + " : " + std::to_string(100 - i * (std::rand() % 30) - std::rand() % 10) + "%");
        ui->match_listWidget->addItem(itemText);
    }

    ui->objects_listWidget->clear();
    for (auto& typ : matchTypes)
    {
        ui->objects_listWidget->addItem(typ.c_str());
    }
}

bool MainWindow::setupDatabase()
{
    const QString setupQuery1 = R"(
        CREATE TABLE IF NOT EXISTS devices(
            id 		integer primary key AUTOINCREMENT not null,
            token 	text UNIQUE,
            name	text,
            config	text,
            place	text
        );
    )";

    const QString setupQuery2 = R"(
        CREATE TABLE IF NOT EXISTS templates (
            id 		integer primary key AUTOINCREMENT not null,
            token	text unique,
            path	text
        );
    )";

    return (m_query.exec(setupQuery1) && m_query.exec(setupQuery2));
}

void MainWindow::loadAllTokens()
{
    if (!m_query.exec(QString("SELECT token FROM devices")))
    {
        qDebug() << "Error in load tokens query:" << m_query.lastError().text();
        return;
    }
    if (!m_query.next())
    {
        qDebug() << "No tokens in database";
        return;
    }
    while (m_query.next()) m_deviceTokens.push_back(m_query.value(0).toString());
    updateDeviceList();
}

QString MainWindow::getDeviceName(const QString &token)
{
    if (!m_query.exec(QString("SELECT name FROM devices WHERE token='%1'").arg(token)))
    {
        qDebug() << "Error in get dev query:" << m_query.lastError().text();
        return "";
    }
    if (!m_query.next())
    {
        qDebug() << "Not found device in database";
        return "";
    }
    return m_query.value(0).toString();
}

QString MainWindow::getDeviceToken(const QString &devName)
{
    if (!m_query.exec(QString("SELECT token FROM devices WHERE name='%1'").arg(devName)))
    {
        qDebug() << "Error in get dev query:" << m_query.lastError().text();
        return "";
    }
    if (!m_query.next())
    {
        qDebug() << "Not found device in database";
        return "";
    }
    return m_query.value(0).toString();
}

ConnectedDevice MainWindow::loadDeviceFromDatabase(const QString &token)
{
    ConnectedDevice result;
    if (!m_query.exec(QString("SELECT name, config, place FROM devices WHERE token='%1'").arg(token)))
    {
        qDebug() << "Error in load dev query:" << m_query.lastError().text();
        return result;
    }
    if (!m_query.next())
    {
        qDebug() << "Not found device in database";
        return result;
    }
    result.token = token;
    result.name = m_query.value(0).toString();
    result.config = m_query.value(1).toString();
    result.place = m_query.value(2).toString();
    return result;
}

void MainWindow::uploadDeviceToDatabase(const ConnectedDevice &dev)
{
    if (!m_query.exec(QString("INSERT INTO devices(token, name, config, place) VALUES ('%1', '%2', '%3', '%4')").arg(dev.token, dev.name, dev.config, dev.place)))
    {
        qDebug() << "Error in upload query:" << m_query.lastError().text();
        return;
    }
    qDebug() << "Added device to database";
}

void MainWindow::updateDeviceInDatabase(const ConnectedDevice &dev)
{
    if (!m_query.exec(QString("UPDATE devices SET name='%1', config='%2', place='%3' WHERE token='%4'").arg(dev.name, dev.config, dev.place, dev.token)))
    {
        qDebug() << "Error in update query:" << m_query.lastError().text();
        return;
    }
    qDebug() << "Updated device info in database";
}
