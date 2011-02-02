#ifndef WIDGETSMILEYLIST_H
#define WIDGETSMILEYLIST_H

#include <QMenu>
#include <QListWidgetItem>

namespace Ui {
    class WidgetSmileyList;
}

class WidgetSmileyList : public QMenu {
    Q_OBJECT
public:
    WidgetSmileyList(QWidget *parent = 0);
    ~WidgetSmileyList();

protected:
	void changeEvent(QEvent *e);

private:
    Ui::WidgetSmileyList *ui;

signals:
	void smileyClicked(QString smiley);

private slots:
	void on_listWidget_itemClicked(QListWidgetItem* item);
};

#endif // WIDGETSMILEYLIST_H
