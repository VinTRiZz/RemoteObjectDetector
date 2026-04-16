#pragma once

#include <QWidget>

#include "common/serverconfiguration.hpp"

namespace Ui {
class DetectorManagementForm;
}

class DetectorListModel;
class DetectorInfoManager;

class DetectorManagementForm : public QWidget
{
    Q_OBJECT

public:
    explicit DetectorManagementForm(QWidget *parent = nullptr);
    ~DetectorManagementForm();


public slots:
    void setServer(const ServerConfiguration &conf);

private:
    Ui::DetectorManagementForm *ui;

    DetectorInfoManager*    m_pDetectorInfoManager {nullptr};
    DetectorListModel*      m_pDetectorListModel {nullptr};
};

