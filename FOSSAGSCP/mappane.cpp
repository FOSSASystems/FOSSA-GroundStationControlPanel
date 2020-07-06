#include "mappane.h"
#include "ui_mappane.h"

MapPane::MapPane(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MapPane)
{
    ui->setupUi(this);

    ui->quickWidget->setSource(QUrl("qrc:/qml/MapBoxQMLQuick.qml"));

}

MapPane::~MapPane()
{
    delete ui;
}
