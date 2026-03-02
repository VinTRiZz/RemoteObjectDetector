#include "servermanagementform.hpp"
#include "ui_servermanagementform.h"

ServerManagementForm::ServerManagementForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ServerManagementForm)
{
    ui->setupUi(this);
}

ServerManagementForm::~ServerManagementForm()
{
    delete ui;
}
