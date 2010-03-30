#ifndef DIALOGDOWNLOADSIMPORT_H
#define DIALOGDOWNLOADSIMPORT_H

#include <QDialog>

namespace Ui {
    class DialogDownloadsImport;
}

class DialogDownloadsImport : public QDialog {
    Q_OBJECT
public:
    DialogDownloadsImport(QWidget *parent = 0);
    ~DialogDownloadsImport();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::DialogDownloadsImport *ui;
};

#endif // DIALOGDOWNLOADSIMPORT_H
