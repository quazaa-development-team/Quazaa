#ifndef WIDGETTRANSFERS_H
#define WIDGETTRANSFERS_H

#include <QWidget>
#include "widgetdownloads.h"
#include "widgetuploads.h"

namespace Ui {
    class WidgetTransfers;
}

class WidgetTransfers : public QWidget {
    Q_OBJECT
public:
    WidgetTransfers(QWidget *parent = 0);
    ~WidgetTransfers();
	WidgetDownloads *panelDownloads;
	WidgetUploads *panelUploads;
	void saveWidget();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WidgetTransfers *ui;

private slots:
	void on_toolButtonDownloadsHeader_clicked();
 void on_toolButtonUploadsHeader_clicked();
 void on_splitterTransfers_customContextMenuRequested(QPoint pos);
 void skinChangeEvent();
};

#endif // WIDGETTRANSFERS_H
