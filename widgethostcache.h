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
	void saveState();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WidgetHostCache *ui;

private slots:
	void skinChangeEvent();
};

#endif // WIDGETHOSTCACHE_H
