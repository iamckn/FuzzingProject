#include "aboutd.h"
#include "ui_aboutd.h"

aboutD::aboutD(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::aboutD)
{
    ui->setupUi(this);
    this->setLayout(ui->verticalLayout);
}

aboutD::~aboutD()
{
    delete ui;
}
