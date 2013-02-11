#ifndef DIALOGIRCSETTINGS_H
#define DIALOGIRCSETTINGS_H

#include <QDialog>
#include <QWizardPage>

#include <QSortFilterProxyModel>
#include <QStandardItemModel>

namespace ColorColumns {
	enum Columns {
		Message,
		Color
	};
}

namespace ShortcutColumns {
	enum Columns {
		Description,
		Shortcut
	};
}

namespace AliasColumns {
    enum Columns {
        Alias,
        Command
    };
}

namespace Ui {
class DialogIrcSettings;
}

class DialogIrcSettings : public QDialog
{
	Q_OBJECT

public:
	explicit DialogIrcSettings(QWidget *parent = 0);
	~DialogIrcSettings();

private:
	Ui::DialogIrcSettings *ui;

    QModelIndex addAliasRow(const QString& alias, const QString& command);

    QStandardItemModel* sourceModel;
    QSortFilterProxyModel* proxyModel;

public slots:
	void saveSettings();
	void loadSettings();

private slots:
	void on_pushButtonOK_clicked();
	void enableApply();
    void addAlias();
    void onAliasClicked(const QModelIndex& index);
};

#endif // DIALOGIRCSETTINGS_H
