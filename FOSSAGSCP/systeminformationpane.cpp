#include "systeminformationpane.h"
#include "ui_systeminformationpane.h"

systeminformationpane::systeminformationpane(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::systeminformationpane)
{
    ui->setupUi(this);
}

systeminformationpane::~systeminformationpane()
{
    delete ui;
}
