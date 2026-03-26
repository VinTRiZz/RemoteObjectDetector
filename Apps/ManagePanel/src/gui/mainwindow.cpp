#include "mainwindow.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->serverManagementForm, &ServerManagementForm::serverSelected,
            ui->detectorManagementForm, &DetectorManagementForm::setServer);

    // DEBUG
    ui->serverManagementForm->addServer("Локальный", "127.0.0.1:9001");
}

MainWindow::~MainWindow()
{
    delete ui;
}
