#include "dialogneighbourinfo.h"
#include "ui_dialogneighbourinfo.h"

DialogNeighbourInfo::DialogNeighbourInfo(CNeighboursTableModel::Neighbour *pNeighbour, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogNeighbourInfo)
{
    ui->setupUi(this);


}

DialogNeighbourInfo::~DialogNeighbourInfo()
{
    delete ui;
}
