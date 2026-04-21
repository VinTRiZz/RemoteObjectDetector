#pragma once

#include <QDialog>

#include "common/serverconfiguration.hpp"

namespace Ui {
class ServerAddDialog;
}

class ServerAddDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ServerAddDialog(QWidget *parent = nullptr);
    ~ServerAddDialog();

    ServerConfiguration getConfig();

private:
    Ui::ServerAddDialog *ui;
};
