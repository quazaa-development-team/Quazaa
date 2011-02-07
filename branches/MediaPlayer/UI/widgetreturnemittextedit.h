#ifndef WIDGETRETURNEMITTEXTEDIT_H
#define WIDGETRETURNEMITTEXTEDIT_H

#include <QTextEdit>
#include <QKeyEvent>

class WidgetReturnEmitTextEdit : public QTextEdit
{
Q_OBJECT
public:
	explicit WidgetReturnEmitTextEdit(QWidget *parent = 0);

private:
	bool emitReturn;

protected:
	void keyPressEvent(QKeyEvent* event);

signals:
	void returnPressed();

public slots:
	void setEmitsReturn(bool shouldEmit);
	bool emitsReturn();
};

#endif // WIDGETRETURNEMITTEXTEDIT_H
