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

    void setCreateMode(bool isCreateMode);

    void showConfiguration(const DataObjects::DetectorConfiguration& conf);
    DataObjects::DetectorConfiguration readConfiguration();

private:
    Ui::DetectorConfigurationWidget *ui;

    DataObjects::DetectorConfiguration m_conf;
};
