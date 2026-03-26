#pragma once

#include <QWidget>

namespace Ui {
class DetectorStreamWidget;
}

class DetectorStreamWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DetectorStreamWidget(QWidget *parent = nullptr);
    ~DetectorStreamWidget();

private:
    Ui::DetectorStreamWidget *ui;
};
