#pragma once

#include <QWidget>

#include <ROD/Protocol.h>
#include <ROD/DeviceStatus.h>

namespace Ui {
class SystemStatusWidget;
}

class SystemStatusWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SystemStatusWidget(QWidget *parent = nullptr);
    ~SystemStatusWidget();

    void setInvalidState();
    void setDisplayInfo(const DataObjects::DeviceStatus& status);

signals:
    void requestedPoweroff();
    void requestedReboot();

private:
    Ui::SystemStatusWidget *ui;
};
