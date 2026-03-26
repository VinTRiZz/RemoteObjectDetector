#include "detectorstreamwidget.hpp"
#include "ui_detectorstreamwidget.h"

DetectorStreamWidget::DetectorStreamWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DetectorStreamWidget)
{
    ui->setupUi(this);
}

DetectorStreamWidget::~DetectorStreamWidget()
{
    delete ui;
}
