#include "serveradddialog.hpp"
#include "ui_serveradddialog.h"

#include <QRegularExpressionValidator>

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

ServerConfiguration ServerAddDialog::getConfig()
{
    ServerConfiguration res;

    auto addr = ui->lineEditServerAddress->text();
    auto addrParts = addr.split(":");

    res.setAddress(addrParts.front()); // Must be anytime
    if (addrParts.size() > 1) {
        res.setPort(addrParts[1].toInt());
    }

    res.setName(ui->lineEditServerName->text());
    return res;
}
