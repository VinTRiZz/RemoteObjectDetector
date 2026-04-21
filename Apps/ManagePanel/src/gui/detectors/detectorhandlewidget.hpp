#pragma once

#include <QWidget>

#include "client/handlers.hpp"

namespace Ui {
class DetectorHandleWidget;
}

class DetectorHandleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DetectorHandleWidget(QWidget *parent = nullptr);
    ~DetectorHandleWidget();

    void setDetector(const Web::DetectorHandler& hdl);

private:
    Ui::DetectorHandleWidget *ui;

    Web::DetectorHandler m_detector;
};
