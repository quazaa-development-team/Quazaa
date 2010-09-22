#ifndef WIDGETCHATTAB_H
#define WIDGETCHATTAB_H

#include <QWidget>
#include <QStringListModel>

namespace Ui {
	class WidgetChatTab;
}

class WidgetChatTab : public QWidget
{
    Q_OBJECT
public:
	WidgetChatTab(QWidget *parent = 0);
	~WidgetChatTab();
	//WidgetChatTab *panelChatTab;
	void saveWidget();
	void append(QString text);
	void setName(QString str);
	void channelNames(QStringList names);
	QString name;

private:
	Ui::WidgetChatTab *ui;
	QStringListModel *userList;

private slots:
	void skinChangeEvent();

};

#endif // WIDGETCHATTAB_H
