#include "mainwindow.h"
#include "ui_mainwindow.h"


void MainWindow::on_systemMenu_pushButton_clicked()
{
    ui->menu_stackedWidget->setCurrentIndex(0);
}


void MainWindow::on_typeManagerMenu_pushButton_clicked()
{
    ui->menu_stackedWidget->setCurrentIndex(1);
}


void MainWindow::on_monitor_pushButton_clicked()
{
    ui->menu_stackedWidget->setCurrentIndex(2);
}


void MainWindow::on_init_pushButton_clicked()
{
    m_server->setup(m_currentDevice.token);
    emit addMessageToHistory("Device init called");
}


void MainWindow::on_viewEnable_pushButton_clicked()
{
#warning "Fals"
//    m_currentDevice.cameraEnabled = true;
//    ui->camera_label->show();
    emit addMessageToHistory("Camera enabled");
}


void MainWindow::on_viewDisable_pushButton_clicked()
{
    m_imageIsLoadingNow = false;
    m_currentDevice.cameraEnabled = false;
    ui->camera_label->hide();
    emit addMessageToHistory("Camera disabled");
}


void MainWindow::on_start_pushButton_clicked()
{
    m_server->start(m_currentDevice.token);
    emit addMessageToHistory("Device start requested");
}


void MainWindow::on_stop_pushButton_clicked()
{
    m_server->stop(m_currentDevice.token);
    emit addMessageToHistory("Device stop requested");
}


void MainWindow::on_reboot_pushButton_clicked()
{
    m_server->reboot(m_currentDevice.token);
    emit addMessageToHistory("Device reboot requested");
}


void MainWindow::on_addObject_pushButton_clicked()
{
    auto objectName = ui->objectName_lineEdit->text();
    if (!objectName.size())
    {
        emit addMessageToHistory("Error: no name written");
        return;
    }
    m_server->addObject(objectName, m_currentDevice.token);
    emit addMessageToHistory(QString("Add object %1 requested").arg(objectName));
}


void MainWindow::on_objects_listWidget_itemClicked(QListWidgetItem *item)
{
    ui->objectName_lineEdit->setText(item->text());
}


void MainWindow::on_removeObject_pushButton_clicked()
{
    auto pItem = ui->objects_listWidget->currentItem();
    if (!pItem)
    {
        emit addMessageToHistory("Error: no object selected");
        return;
    }
    m_server->removeObject(pItem->text(), m_currentDevice.token);
    emit addMessageToHistory(QString("Remove object %1 requested").arg(pItem->text()));
}


void MainWindow::on_renameObject_pushButton_clicked()
{
    auto pItem = ui->objects_listWidget->currentItem();
    if (!pItem)
    {
        emit addMessageToHistory("Error: no object selected");
        return;
    }
    auto newName = ui->objectName_lineEdit->text();
    if (!newName.size())
    {
        emit addMessageToHistory("Error: no name written");
        return;
    }
    m_server->renameObject(pItem->text(), newName, m_currentDevice.token);
    emit addMessageToHistory(QString("Rename object %1 --> %2 requested").arg(pItem->text(), newName));
}
