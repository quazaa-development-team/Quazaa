#ifndef DIALOGLIBRARYSEARCH_H
#define DIALOGLIBRARYSEARCH_H

#include <QDialog>

namespace Ui {
	class DialogLibrarySearch;
}

class DialogLibrarySearch : public QDialog {
	Q_OBJECT
public:
	DialogLibrarySearch(QWidget *parent = 0);
	~DialogLibrarySearch();

protected:
	void changeEvent(QEvent *e);

private:
	Ui::DialogLibrarySearch *ui;

private slots:
	void on_pushButtonCancel_clicked();
	void on_pushButtonSearch_clicked();
};

#endif // DIALOGLIBRARYSEARCH_H
