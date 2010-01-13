#ifndef DIALOGPREVIEWPREPARE_H
#define DIALOGPREVIEWPREPARE_H

#include <QtGui/QDialog>

namespace Ui {
	class DialogPreviewPrepare;
}

class DialogPreviewPrepare : public QDialog {
	Q_OBJECT
public:
	DialogPreviewPrepare(QWidget *parent = 0);
	~DialogPreviewPrepare();
	void updateProgress(int percent, QString fileName);

protected:
	void changeEvent(QEvent *e);

private:
	Ui::DialogPreviewPrepare *m_ui;

private slots:
	void on_pushButton_clicked();
};

#endif // DIALOGPREVIEWPREPARE_H
