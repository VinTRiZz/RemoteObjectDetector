#pragma once

#include <QWidget>

namespace Ui {
class DetectionObjectStatusWidget;
}

class DetectionObjectStatusWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DetectionObjectStatusWidget(QWidget *parent = nullptr);
    ~DetectionObjectStatusWidget();

private:
    Ui::DetectionObjectStatusWidget *ui;
};
