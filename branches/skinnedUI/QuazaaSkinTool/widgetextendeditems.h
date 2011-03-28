#ifndef WIDGETEXTENDEDITEMS_H
#define WIDGETEXTENDEDITEMS_H

#include <QtGui>

namespace Ui
{
	class WidgetExtendedItems;
}

class WidgetExtendedItems : public QMainWindow
{
	Q_OBJECT
public:
	WidgetExtendedItems(QWidget* parent = 0);
	~WidgetExtendedItems();
	QSlider* seekSlider;
	QSlider* volumeSlider;
	QLabel* labelSeekSlider;
	QLabel* labelVolumeSlider;

protected:
	void changeEvent(QEvent* e);

private:
	Ui::WidgetExtendedItems* ui;

public slots:
	void skinChangeEvent();
};

#endif // WIDGETEXTENDEDITEMS_H
