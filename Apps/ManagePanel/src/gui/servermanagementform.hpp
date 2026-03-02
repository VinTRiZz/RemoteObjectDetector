#pragma once

#include <QWidget>

namespace Ui {
class ServerManagementForm;
}

class ServerManagementForm : public QWidget
{
    Q_OBJECT

public:
    explicit ServerManagementForm(QWidget *parent = nullptr);
    ~ServerManagementForm();

private:
    Ui::ServerManagementForm *ui;
};

