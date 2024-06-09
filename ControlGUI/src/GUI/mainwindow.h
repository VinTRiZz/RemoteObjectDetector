#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QStandardItemModel>
#include <QTimer>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include <nlohmann/json.hpp>

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

    void objectListGot(const QString& objectList);
    void objectDetectedListGot(const QString& objectDetectedList);
    void objectAdded(const QString& objectName);
    void objectRenamed(const QString& objectName, const QString& newName);
    void objectRemoved(const QString& objectName);

    void deviceStatusGot(const Exchange::StatusData& devStatus);

    // UI things
    void addMessageToHistory(const QString& messageText);

    // Periodic requests for choosen device
    void periodicRequest();

    void on_init_pushButton_clicked();

    void on_viewEnable_pushButton_clicked();

    void on_viewDisable_pushButton_clicked();

    void on_start_pushButton_clicked();

    void on_stop_pushButton_clicked();

    void on_reboot_pushButton_clicked();

    void on_addObject_pushButton_clicked();

    void on_objects_listWidget_itemClicked(QListWidgetItem *item);

    void on_removeObject_pushButton_clicked();

    void on_renameObject_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QStandardItemModel* m_pStatusModel;
    ControlServer* m_server;
    QTimer* m_requestTimer;

    QSqlDatabase m_db;
    QSqlQuery m_query;

    std::list<QString> m_deviceTokens;
    ConnectedDevice m_currentDevice;

    uint64_t m_updateTime {1000};

    bool setupDatabase();
    void loadAllTokens();
    QString getDeviceName(const QString& token);
    ConnectedDevice loadDeviceFromDatabase(const QString& token);
    void uploadDeviceToDatabase(const ConnectedDevice& dev);
    void updateDeviceInDatabase(const ConnectedDevice& dev);

    void updateDeviceList();
    void setDevice(const QString &devToken);
    void cleanDeviceContent();
};
#endif // MAINWINDOW_H
