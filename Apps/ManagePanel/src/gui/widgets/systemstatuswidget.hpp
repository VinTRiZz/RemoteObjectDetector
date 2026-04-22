#pragma once

#include <QWidget>

#include <ROD/DataObjects/DeviceStatus.h>

namespace Ui {
class SystemStatusWidget;
}

class SystemStatusWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SystemStatusWidget(QWidget *parent = nullptr);
    ~SystemStatusWidget();

    void setStatus(const DataObjects::DeviceStatus& status);

signals:
    void requestUpdateInfo(); // For timed status updates

    void requestedPoweroff();
    void requestedReboot();

private:
    Ui::SystemStatusWidget *ui;

    // Status update
    unsigned    m_timerUpdateTimeMs {1000};
    bool        m_isUpdatesCalled {false};

    void restartUpdateTimer();

protected:
    void showEvent(QShowEvent* e) override;
};
