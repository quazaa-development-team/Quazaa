#ifndef DIALOGNEWSKIN_H
#define DIALOGNEWSKIN_H

#include <QDialog>

namespace Ui {
	class DialogNewSkin;
}

class DialogNewSkin : public QDialog {
	Q_OBJECT
public:
	DialogNewSkin(QWidget *parent = 0);
	~DialogNewSkin();
	QString name;
	QString author;
	QString version;
	QString description;

protected:
	void changeEvent(QEvent *e);

private:
	Ui::DialogNewSkin *ui;

private slots:
	void on_pushButtonCancel_clicked();
	void on_pushButtonOK_clicked();
};

#endif // DIALOGNEWSKIN_H
