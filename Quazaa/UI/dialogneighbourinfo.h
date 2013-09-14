#ifndef DIALOGNEIGHBOURINFO_H
#define DIALOGNEIGHBOURINFO_H

#include "neighbourstablemodel.h"
#include <QDialog>

namespace Ui {
class DialogNeighbourInfo;
}

class DialogNeighbourInfo : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogNeighbourInfo(CNeighboursTableModel::Neighbour* pNeighbour, QWidget *parent = 0);
    ~DialogNeighbourInfo();
    
private:
    Ui::DialogNeighbourInfo *ui;
};

#endif // DIALOGNEIGHBOURINFO_H
