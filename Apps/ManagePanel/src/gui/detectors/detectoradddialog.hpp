#pragma once

#include <QDialog>

#include <ROD/DataObjects/DetectorConfiguration.h>

namespace Ui {
class DetectorAddDialog;
}

class DetectorAddDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DetectorAddDialog(QWidget *parent = nullptr);
    ~DetectorAddDialog();

    DataObjects::DetectorConfiguration readConfiguration() const;

private:
    Ui::DetectorAddDialog *ui;
};
