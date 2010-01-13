#ifndef DIALOGCLOSETYPE_H
#define DIALOGCLOSETYPE_H

#include <QtGui/QDialog>

namespace Ui {
	class DialogCloseType;
}

class DialogCloseType : public QDialog {
	Q_OBJECT
public:
	DialogCloseType(QWidget *parent = 0);
	~DialogCloseType();

protected:
	void changeEvent(QEvent *e);

private:
	Ui::DialogCloseType *m_ui;

private slots:
	void on_pushButtonOK_clicked();
};

#endif // DIALOGCLOSETYPE_H
