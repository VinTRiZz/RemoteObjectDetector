#include "serveradddialog.hpp"
#include "ui_serveradddialog.h"

ServerAddDialog::ServerAddDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ServerAddDialog)
{
    ui->setupUi(this);
}

ServerAddDialog::~ServerAddDialog()
{
    delete ui;
}
