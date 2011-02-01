#include "widgetreturnemittextedit.h"

WidgetReturnEmitTextEdit::WidgetReturnEmitTextEdit(QWidget *parent)
{
	emitReturn = true;
}

void WidgetReturnEmitTextEdit::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Return)
	{
		if (emitReturn)
		{
			emit returnPressed();
		} else {
			QTextEdit::keyPressEvent(event);
		}
	} else {
		QTextEdit::keyPressEvent(event);
	}
}

void WidgetReturnEmitTextEdit::setEmitsReturn(bool shouldEmit)
{
	emitReturn = shouldEmit;
}


bool WidgetReturnEmitTextEdit::emitsReturn()
{
	return emitReturn;
}
