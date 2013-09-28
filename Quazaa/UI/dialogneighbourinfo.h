#ifndef DIALOGNEIGHBOURINFO_H
#define DIALOGNEIGHBOURINFO_H

#include "neighbourstablemodel.h"
#include <QDialog>

namespace Ui {
class CDialogNeighbourInfo;
}

class CDialogNeighbourInfo : public QDialog
{
	Q_OBJECT

public:
	explicit CDialogNeighbourInfo(CNeighboursTableModel::Neighbour* pNeighbour, QWidget *parent = 0);
	~CDialogNeighbourInfo();

private:
	Ui::CDialogNeighbourInfo *ui;
	QString neighbourConnectionDescription(CNeighboursTableModel::Neighbour* pNeighbour);
};

#endif // DIALOGNEIGHBOURINFO_H
