#pragma once

#include <QWidget>

namespace Ui {
class DetectorManagementForm;
}

class DetectorManagementForm : public QWidget
{
    Q_OBJECT

public:
    explicit DetectorManagementForm(QWidget *parent = nullptr);
    ~DetectorManagementForm();

private:
    Ui::DetectorManagementForm *ui;
};

