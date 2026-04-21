#include "detectorhandlewidget.hpp"
#include "ui_detectorhandlewidget.h"

DetectorHandleWidget::DetectorHandleWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DetectorHandleWidget)
{
    ui->setupUi(this);

    ui->streamingWidget->hide(); // TODO: Disable streaming without explicit starting
}

DetectorHandleWidget::~DetectorHandleWidget()
{
    delete ui;
}

void DetectorHandleWidget::setDetector(const Web::DetectorHandler &hdl)
{
    // TODO: Show data
}
