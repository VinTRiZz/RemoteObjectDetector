#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>

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
    void removeConnection(const QString devToken);

    void addMessageToHistory(const QString& messageText);


private:
    Ui::MainWindow *ui;
    QStandardItemModel* m_pStatusModel;
    ControlServer* m_server;

    std::list<ConnectedDevice> m_devices;

    void updateDeviceList();
    void setDevice(const QString &devName);
    void startTestFunction();
};
#endif // MAINWINDOW_H
