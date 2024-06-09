#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QTimer>

#include "commonstructs.h"
#include "controlserver.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_systemMenu_pushButton_clicked();

    void on_typeManagerMenu_pushButton_clicked();

    void on_monitor_pushButton_clicked();

    // Server slots
    void addConnection(const QString &devToken);
    void removeConnection(const QString& devToken);
    void deviceIsReady(const QString& devToken);
    void deviceStarted(const QString& devToken);
    void deviceStopped(const QString& devToken);
    void objectAdded(const QString& objectName);
    void objectRenamed(const QString& objectName, const QString& newName);
    void objectRemoved(const QString& objectName);
    void deviceStatusGot(const Exchange::StatusData& devStatus);

    // UI things
    void addMessageToHistory(const QString& messageText);

    // Periodic requests for choosen device
    void periodicRequest();

private:
    Ui::MainWindow *ui;
    QStandardItemModel* m_pStatusModel;
    ControlServer* m_server;
    QTimer* m_requestTimer;

    std::list<QString> m_deviceTokens;
    ConnectedDevice m_currentDevice;

    uint64_t m_updateTime {1000};
    bool m_periodicUpdateStatus {false};

    void updateDeviceList();
    void setDevice(const QString &devToken);
    void startTestFunction();
};
#endif // MAINWINDOW_H
