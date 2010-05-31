#ifndef WIDGETHOSTCACHE_H
#define WIDGETHOSTCACHE_H

#include <QMainWindow>

namespace Ui {
    class WidgetHostCache;
}

class WidgetHostCache : public QMainWindow {
    Q_OBJECT
public:
    WidgetHostCache(QWidget *parent = 0);
    ~WidgetHostCache();
	void saveWidget();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WidgetHostCache *ui;

private slots:
	void on_splitterHostCache_customContextMenuRequested(QPoint pos);
 void skinChangeEvent();
};

#endif // WIDGETHOSTCACHE_H
