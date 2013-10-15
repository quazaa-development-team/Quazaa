#include "dialogircsettings.h"
#include "ui_dialogircsettings.h"

#include "quazaasettings.h"

#include <QSortFilterProxyModel>
#include <QStyledItemDelegate>
#include <QStandardItemModel>
#include <QComboBox>
#include <QDebug>

// SVG color keyword names provided by the World Wide Web Consortium
static const QStringList COLORS = QStringList()
		<< "aliceblue" << "antiquewhite" << "aqua" << "aquamarine" << "azure" << "beige" << "bisque"
		<< "black" << "blanchedalmond" << "blue" << "blueviolet" << "brown" << "burlywood" << "cadetblue"
		<< "chartreuse" << "chocolate" << "coral" << "cornflowerblue" << "cornsilk" << "crimson" << "cyan"
		<< "darkblue" << "darkcyan" << "darkgoldenrod" << "darkgray" << "darkgreen" << "darkgrey"
		<< "darkkhaki" << "darkmagenta" << "darkolivegreen" << "darkorange" << "darkorchid" << "darkred"
		<< "darksalmon" << "darkseagreen" << "darkslateblue" << "darkslategray" << "darkslategrey"
		<< "darkturquoise" << "darkviolet" << "deeppink" << "deepskyblue" << "dimgray" << "dimgrey"
		<< "dodgerblue" << "firebrick" << "floralwhite" << "forestgreen" << "fuchsia" << "gainsboro"
		<< "ghostwhite" << "gold" << "goldenrod" << "gray" << "grey" << "green" << "greenyellow"
		<< "honeydew" << "hotpink" << "indianred" << "indigo" << "ivory" << "khaki" << "lavender"
		<< "lavenderblush" << "lawngreen" << "lemonchiffon" << "lightblue" << "lightcoral" << "lightcyan"
		<< "lightgoldenrodyellow" << "lightgray" << "lightgreen" << "lightgrey" << "lightpink"
		<< "lightsalmon" << "lightseagreen" << "lightskyblue" << "lightslategray" << "lightslategrey"
		<< "lightsteelblue" << "lightyellow" << "lime" << "limegreen" << "linen" << "magenta"
		<< "maroon" << "mediumaquamarine" << "mediumblue" << "mediumorchid" << "mediumpurple"
		<< "mediumseagreen" << "mediumslateblue" << "mediumspringgreen" << "mediumturquoise"
		<< "mediumvioletred" << "midnightblue" << "mintcream" << "mistyrose" << "moccasin"
		<< "navajowhite" << "navy" << "oldlace" << "olive" << "olivedrab" << "orange" << "orangered"
		<< "orchid" << "palegoldenrod" << "palegreen" << "paleturquoise" << "palevioletred"
		<< "papayawhip" << "peachpuff" << "peru" << "pink" << "plum" << "powderblue" << "purple" << "red"
		<< "rosybrown" << "royalblue" << "saddlebrown" << "salmon" << "sandybrown" << "seagreen"
		<< "seashell" << "sienna" << "silver" << "skyblue" << "slateblue" << "slategray" << "slategrey"
		<< "snow" << "springgreen" << "steelblue" << "tan" << "teal" << "thistle" << "tomato"
		<< "turquoise" << "violet" << "wheat" << "white" << "whitesmoke" << "yellow" << "yellowgreen";


class ColorItemDelegate : public QStyledItemDelegate
{
public:
	ColorItemDelegate(QObject* parent = 0) : QStyledItemDelegate(parent) {
	}

	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const {
		Q_UNUSED(option);
		if (index.column() == ColorColumns::Color) {
			QComboBox* comboBox = new QComboBox(parent);
			comboBox->addItems(COLORS);
			int i = 0;
			foreach(const QString & color, COLORS)
			{
				comboBox->setItemData(i++, QColor(color), Qt::DecorationRole);
			}
			return comboBox;
		}
		return 0;
	}

	void setEditorData(QWidget* editor, const QModelIndex& index) const {
		if (index.column() == ColorColumns::Color) {
			QComboBox* comboBox = static_cast<QComboBox*>(editor);
			int i = comboBox->findText(index.data().toString());
			comboBox->setCurrentIndex(i);
		}
	}

	void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const {
		if (index.column() == ColorColumns::Color) {
			QComboBox* comboBox = static_cast<QComboBox*>(editor);
			model->setData(index, comboBox->currentText());
			model->setData(index, QColor(comboBox->currentText()), Qt::DecorationRole);
		}
	}
};

class ShortcutItemDelegate : public QStyledItemDelegate
{
public:
	ShortcutItemDelegate(QObject* parent = 0) : QStyledItemDelegate(parent) { }
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& opt, const QModelIndex& index) const {
		QWidget* editor = 0;
		if (index.column() == ShortcutColumns::Shortcut)
			editor = QStyledItemDelegate::createEditor(parent, opt, index);
		return editor;
	}
};

class SortFilterProxyModel : public QSortFilterProxyModel
{
public:
	SortFilterProxyModel(QObject* parent = 0) : QSortFilterProxyModel(parent)
	{
	}

protected:
	bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
	{
		QModelIndex aliasIndex = sourceModel()->index(sourceRow, AliasColumns::Alias, sourceParent);
		QModelIndex commandIndex = sourceModel()->index(sourceRow, AliasColumns::Command, sourceParent);
		QString alias, command;
		if (!aliasIndex.isValid() || !commandIndex.isValid()) // the column may not exist
			alias = aliasIndex.data(filterRole()).toString();
		command = commandIndex.data(filterRole()).toString();

		return alias.contains(filterRegExp()) || command.contains(filterRegExp());
		return false;
	}
};

class CloseDelegate : public QStyledItemDelegate
{
public:
	CloseDelegate(QObject* parent = 0) : QStyledItemDelegate(parent)
	{
	}

	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		const_cast<QStyleOptionViewItem&>(option).decorationPosition = QStyleOptionViewItem::Right;
		QStyledItemDelegate::paint(painter, option, index);
	}
};

CDialogIrcSettings::CDialogIrcSettings(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::CDialogIrcSettings)
{
	ui->setupUi(this);

	setWindowFilePath(tr("Settings"));

	ui->treeWidgetColors->setItemDelegate(new ColorItemDelegate(ui->treeWidgetColors));
	ui->treeWidgetShortcuts->setItemDelegate(new ShortcutItemDelegate(ui->treeWidgetShortcuts));
#if QT_VERSION >= 0x050000
	ui->treeWidgetColors->header()->setSectionResizeMode(ColorColumns::Message, QHeaderView::ResizeToContents);
	ui->treeWidgetShortcuts->header()->setSectionResizeMode(ShortcutColumns::Description, QHeaderView::ResizeToContents);
#else
	ui->treeWidgetColors->header()->setResizeMode(ColorColumns::Message, QHeaderView::ResizeToContents);
	ui->treeWidgetShortcuts->header()->setResizeMode(ShortcutColumns::Description, QHeaderView::ResizeToContents);
#endif
	ui->treeWidgetShortcuts->expandItem(ui->treeWidgetShortcuts->topLevelItem(0));
	ui->treeWidgetShortcuts->expandItem(ui->treeWidgetShortcuts->topLevelItem(1));
	proxyModel = new SortFilterProxyModel(ui->treeViewAliases);
	sourceModel = new QStandardItemModel(proxyModel);
	proxyModel->setSourceModel(sourceModel);
	ui->treeViewAliases->setModel(proxyModel);
	ui->treeViewAliases->setItemDelegateForColumn(2, new CloseDelegate(ui->treeViewAliases));

	loadSettings();

	connect(ui->checkBoxConnectOnStartup, SIGNAL(toggled(bool)), this, SLOT(enableApply()));
	connect(ui->checkBoxStripNicks, SIGNAL(toggled(bool)), this, SLOT(enableApply()));
	connect(ui->checkBoxTimeStamp, SIGNAL(toggled(bool)), this, SLOT(enableApply()));
	connect(ui->checkBoxDarkTheme, SIGNAL(toggled(bool)), this, SLOT(enableApply()));
	connect(ui->spinBoxBlockCount, SIGNAL(valueChanged(int)), this, SLOT(enableApply()));
	connect(ui->treeWidgetColors, SIGNAL(clicked(QModelIndex)), this, SLOT(enableApply()));
	connect(ui->treeWidgetShortcuts, SIGNAL(clicked(QModelIndex)), this, SLOT(enableApply()));
	connect(ui->toolButtonAddAliases, SIGNAL(clicked()), this, SLOT(addAlias()));
	connect(ui->treeViewAliases, SIGNAL(clicked(QModelIndex)), this, SLOT(onAliasClicked(QModelIndex)));
	connect(ui->treeViewAliases, SIGNAL(activated(QModelIndex)), this, SLOT(enableApply()));
	connect(ui->lineEditAliasesFilter, SIGNAL(textEdited(QString)), proxyModel, SLOT(setFilterWildcard(QString)));

	connect(ui->pushButtonApply, SIGNAL(clicked()), this, SLOT(saveSettings()));
	ui->pushButtonApply->setEnabled(false);
}

CDialogIrcSettings::~CDialogIrcSettings()
{
	delete ui;
}

QModelIndex CDialogIrcSettings::addAliasRow(const QString &alias, const QString &command)
{
	QStandardItem* aliasItem = new QStandardItem(alias);
	QStandardItem* commandItem = new QStandardItem(command);
	QStandardItem* closeItem = new QStandardItem(QPixmap(":/Resource/Generic/Exit.png"), QString());
	closeItem->setFlags(closeItem->flags() & ~Qt::ItemIsEditable);
	sourceModel->appendRow(QList<QStandardItem*>() << aliasItem << commandItem << closeItem);
	return proxyModel->mapFromSource(aliasItem->index());
}

void CDialogIrcSettings::saveSettings()
{
	quazaaSettings.Chat.ConnectOnStartup = ui->checkBoxConnectOnStartup->isChecked();
	quazaaSettings.Chat.Scrollback = ui->spinBoxBlockCount->value();
	quazaaSettings.Chat.ShowTimestamp = ui->checkBoxTimeStamp->isChecked();
	quazaaSettings.Chat.TimestampFormat = ui->lineEditTimestampFormat->text();

	quazaaSettings.Chat.DarkTheme = ui->checkBoxDarkTheme->isChecked();
	QHash<int, QString> colors;
	for (int i = IrcColorType::Background; i <= IrcColorType::LightGray; ++i)
		colors[i] = ui->treeWidgetColors->topLevelItem(i)->data(ColorColumns::Color, Qt::DisplayRole).toString();
	quazaaSettings.Chat.Colors = colors;

	for (int i = IrcShortcutType::NextView; i <= IrcShortcutType::MostActiveView; ++i) {
		QTreeWidgetItem* item = ui->treeWidgetShortcuts->topLevelItem(i);
		quazaaSettings.Chat.Shortcuts.insert(i, item->text(ShortcutColumns::Shortcut));
	}

	QMap<QString, QString> aliases;
	for (int i = 0; i < sourceModel->rowCount(); ++i)
	{
		QString alias = sourceModel->item(i, AliasColumns::Alias)->text();
		QString command = sourceModel->item(i, AliasColumns::Command)->text();
		if (!alias.isEmpty() && !command.isEmpty())
			aliases[alias] = command;
	}
	quazaaSettings.Chat.Aliases = aliases;

	quazaaSettings.saveChat();

	ui->pushButtonApply->setEnabled(false);
}

void CDialogIrcSettings::loadSettings()
{
	ui->checkBoxConnectOnStartup->setChecked(quazaaSettings.Chat.ConnectOnStartup);
	ui->spinBoxBlockCount->setValue(quazaaSettings.Chat.Scrollback);
	ui->checkBoxTimeStamp->setChecked(quazaaSettings.Chat.ShowTimestamp);
	ui->lineEditTimestampFormat->setText(quazaaSettings.Chat.TimestampFormat);

	ui->checkBoxDarkTheme->setChecked(quazaaSettings.Chat.DarkTheme);
	QHashIterator<int, QString> hashIt(quazaaSettings.Chat.Colors);
	while (hashIt.hasNext()) {
		hashIt.next();
		ui->treeWidgetColors->topLevelItem(hashIt.key())->setData(ColorColumns::Color, Qt::DisplayRole, hashIt.value());
		ui->treeWidgetColors->topLevelItem(hashIt.key())->setData(ColorColumns::Color, Qt::DecorationRole, QColor(hashIt.value()));
	}

	for (int i = IrcShortcutType::NextView; i <= IrcShortcutType::MostActiveView; ++i) {
		QTreeWidgetItem* item = ui->treeWidgetShortcuts->topLevelItem(i);
		item->setText(ShortcutColumns::Shortcut, quazaaSettings.Chat.Shortcuts.value(i));
	}

	sourceModel->clear();
	sourceModel->setHorizontalHeaderLabels(QStringList() << tr("Alias") << tr("Command") << QString());

	QMapIterator<QString, QString> aliasIt(quazaaSettings.Chat.Aliases);
	while (aliasIt.hasNext())
	{
		aliasIt.next();
		QString alias = aliasIt.key();
		QString command = aliasIt.value();
		//qDebug() << "Alias: " << alias << " Command: " << command;
		if (!alias.isEmpty() && !command.isEmpty())
			addAliasRow(alias, command);
	}

#if QT_VERSION < 0x050000
	ui->treeViewAliases->header()->setResizeMode(0, QHeaderView::ResizeToContents);
	ui->treeViewAliases->header()->setResizeMode(1, QHeaderView::Stretch);
	ui->treeViewAliases->header()->setResizeMode(2, QHeaderView::ResizeToContents);
#else
	ui->treeViewAliases->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	ui->treeViewAliases->header()->setSectionResizeMode(1, QHeaderView::Stretch);
	ui->treeViewAliases->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
#endif
}

void CDialogIrcSettings::on_pushButtonOK_clicked()
{
	if(ui->pushButtonApply->isEnabled())
		saveSettings();
	accept();
}

void CDialogIrcSettings::enableApply()
{
	ui->pushButtonApply->setEnabled(true);
}

void CDialogIrcSettings::addAlias()
{
	ui->lineEditAliasesFilter->clear();
	QModelIndex index = addAliasRow(QString(), QString());
	ui->treeViewAliases->setCurrentIndex(index);
	ui->treeViewAliases->edit(index);
	enableApply();
}

void CDialogIrcSettings::onAliasClicked(const QModelIndex &index)
{
	if (index.column() == 2) {
		qDeleteAll(sourceModel->takeRow(proxyModel->mapToSource(index).row()));
		enableApply();
	}
}
