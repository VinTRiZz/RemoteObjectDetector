#include "detectionobjectstatuswidget.hpp"
#include "ui_detectionobjectstatuswidget.h"

DetectionObjectStatusWidget::DetectionObjectStatusWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DetectionObjectStatusWidget)
{
    ui->setupUi(this);
}

DetectionObjectStatusWidget::~DetectionObjectStatusWidget()
{
    delete ui;
}
