#ifndef DIALOGGPLVIEW_H
#define DIALOGGPLVIEW_H

#include <QtGui/QDialog>
#include <QUrl>

namespace Ui {
	class DialogGPLView;
}

class DialogGPLView : public QDialog {
	Q_OBJECT
public:
	DialogGPLView(QWidget *parent = 0);
	~DialogGPLView();

protected:
	void changeEvent(QEvent *e);

private:
	Ui::DialogGPLView *m_ui;

signals:
	void closed();

private slots:
 void on_textBrowserGPL_anchorClicked(QUrl link);
 void on_pushButtonOK_clicked();
 void skinChangeEvent();
};

#endif // DIALOGGPLVIEW_H
