#ifndef WIDGETNAVIGATION_H
#define WIDGETNAVIGATION_H

#include <QMainWindow>
#include <QActionGroup>

namespace Ui
{
	class WidgetNavigation;
}

class WidgetNavigation : public QMainWindow
{
	Q_OBJECT
public:
	WidgetNavigation(QWidget* parent = 0);
	~WidgetNavigation();
	QActionGroup* actionGroupMainNavigation;

protected:
	void changeEvent(QEvent* e);

private:
	Ui::WidgetNavigation* ui;

public slots:
	void skinChangeEvent();
};

#endif // WIDGETNAVIGATION_H
