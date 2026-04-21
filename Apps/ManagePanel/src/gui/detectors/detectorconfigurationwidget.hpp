#pragma once

#include <QWidget>

#include <ROD/DetectorConfiguration.h>

namespace Ui {
class DetectorConfigurationWidget;
}

class DetectorConfigurationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DetectorConfigurationWidget(QWidget *parent = nullptr);
    ~DetectorConfigurationWidget();

    void setOnlineVisible(bool isOnlineVisible);

    void showConfiguration(const DataObjects::DetectorConfiguration& conf);
    DataObjects::DetectorConfiguration readConfiguration() const;

private:
    Ui::DetectorConfigurationWidget *ui;
};
