#pragma once

#include <QWidget>

#include "client/handlers.hpp"

namespace Ui {
class DetectorManagementForm;
}

class DetectorManagementForm : public QWidget
{
    Q_OBJECT

public:
    explicit DetectorManagementForm(QWidget *parent = nullptr);
    ~DetectorManagementForm();

    void setServer(const Web::ServerHandler& hdl);

private:
    Ui::DetectorManagementForm *ui;

    Web::ServerHandler m_server;
};
