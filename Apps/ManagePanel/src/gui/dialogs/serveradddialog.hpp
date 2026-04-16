#pragma once

#include <QDialog>

namespace Ui {
class ServerAddDialog;
}

class ServerAddDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ServerAddDialog(QWidget *parent = nullptr);
    ~ServerAddDialog();

private:
    Ui::ServerAddDialog *ui;
};
