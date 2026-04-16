#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include "common/serverconfiguration.hpp"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->serverManagementForm, &ServerManagementForm::serverSelected,
            ui->detectorManagementForm, &DetectorManagementForm::setServer);

    // DEBUG
    ServerConfiguration conf;
    conf.setName("Локальный");
    conf.setAddress("127.0.0.1", 9001);
    ui->serverManagementForm->addServer(conf);
}

MainWindow::~MainWindow()
{
    delete ui;
}
