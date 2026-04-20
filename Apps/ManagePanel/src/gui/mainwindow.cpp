#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include "common/serverconfiguration.hpp"
#include "client/serverregistry.hpp"

#include "gui/models/servertreemodel.hpp"
#include "gui/models/detectortreemodel.hpp"

#include <Components/Logger/Logger.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Main context
    m_pServerRegistry = new Web::ServerRegistry(this);

    // Setup server model
    m_pServersModel = new ServerTreeModel(this);
    m_pServersModel->setServerRegistry(m_pServerRegistry);
    ui->treeViewServers->setModel(m_pServersModel);

    // Setup detectors model
    m_pDetectorsModel = new DetectorTreeModel(this);
    ui->treeViewDetectors->setModel(m_pDetectorsModel);

    // After member creating
    setupSignals();
    setupCosmetics();

    // TODO: Move init to other method for init awaiting
    init();

    // TODO: REMOVE AFTER IMPLEMENT ServerRegistry LOCAL DATA SAVING
    ServerConfiguration conf;
    conf.setName("Локальный");
    conf.setAddress("127.0.0.1");
    conf.setPort(9001);
    m_pServerRegistry->addServer(conf);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    // Init main context
    m_pServerRegistry->init();
}

void MainWindow::setupCosmetics()
{
    ui->stackedWidget->setCurrentIndex(0); // Placeholder
}

void MainWindow::setupSignals()
{
    connect(ui->treeViewServers->selectionModel(), &QItemSelectionModel::selectionChanged,
            m_pDetectorsModel, [this](const QItemSelection &selected, const QItemSelection &deselected){
                COMPLOG_DEBUG("CALLED");
                if (selected.empty()) {
                    m_pDetectorsModel->setServer({});
                    ui->stackedWidget->setCurrentIndex(0); // Placeholder
                    return;
                }
                auto serverHdl = m_pServersModel->getServer(selected.indexes().front().row());
                m_pDetectorsModel->setServer(serverHdl);
                ui->stackedWidget->setCurrentIndex(serverHdl.isValid() ? 1 : 0);
            });
}
