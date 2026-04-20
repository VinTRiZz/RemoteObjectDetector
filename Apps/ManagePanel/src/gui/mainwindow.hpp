#pragma once

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class ServerTreeModel;
class DetectorTreeModel;

class ServerManagementForm;


namespace Web {
class ServerRegistry;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void init();

private:
    Ui::MainWindow *ui;

    Web::ServerRegistry* m_pServerRegistry {nullptr};

    ServerTreeModel*    m_pServersModel {nullptr};
    DetectorTreeModel*  m_pDetectorsModel {nullptr};

    // Must be one instance only
    ServerManagementForm* m_pServerManagementForm {nullptr};

    void setupCosmetics();
    void setupSignals();
};

