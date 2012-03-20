#ifndef TABLEVIEW_H
#define TABLEVIEW_H

#include <QKeyEvent>
#include <QAbstractItemModel>
#include <QTableView>

class CTableView : public QTableView
{
public:
	CTableView();

protected:
	// This overwrites certain key events...
	virtual void keyPressEvent(QKeyEvent *event);
};

#endif // TABLEVIEW_H
