//
// mainwindow.cpp
//
// Copyright  Quazaa Development Team, 2009-2010.
// This file is part of QUAZAA (quazaa.sourceforge.net)
//
// Quazaa is free software; you can redistribute it
// and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// Quazaa is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Quazaa; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../QSkinDialog/qskinsettings.h"
#include "dialognewskin.h"
#include "dialogopenskin.h"
#include "quazaacssparser.h"
#include "csshighlighter.h"
#include <QDesktopServices>
#include "qtgradientmanager.h"
#include "qtgradientviewdialog.h"
#include "qtgradientutils.h"
#include <QColorDialog>
#include <QFontDialog>
#include <QFileDialog>
#include <QSignalMapper>
#include <QMessageBox>


MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ui->stackedWidget->setCurrentIndex(0);
	isMainWindow = true;

	pageExtendedItems = new WidgetExtendedItems();
	ui->verticalLayoutExtendedItems->addWidget(pageExtendedItems);
	pageNavigation = new WidgetNavigation();
	ui->verticalLayoutNavigation->addWidget(pageNavigation);

	isMainWindow = true;

	ui->plainTextEditStyleSheet->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui->plainTextEditStyleSheet, SIGNAL(customContextMenuRequested(QPoint)),
				this, SLOT(slotContextMenuRequested()));

	m_addImageAction = new QAction(tr("Add Image..."), this);
	m_addGradientAction = new QAction(tr("Add Gradient..."), this);
	m_addColorAction = new QAction(tr("Add Color..."), this);
	m_addFontAction = new QAction(tr("Add Font..."), this);
	m_addImageAction->setEnabled(false);
	m_addGradientAction->setEnabled(false);
	m_addColorAction->setEnabled(false);
	m_addFontAction->setEnabled(false);

	QSignalMapper *imageActionMapper = new QSignalMapper(this);
	QSignalMapper *gradientActionMapper = new QSignalMapper(this);
	QSignalMapper *colorActionMapper = new QSignalMapper(this);

	imageActionMapper->setMapping(m_addImageAction, QString());
	gradientActionMapper->setMapping(m_addGradientAction, QString());
	colorActionMapper->setMapping(m_addColorAction, QString());

	connect(m_addImageAction, SIGNAL(triggered()), imageActionMapper, SLOT(map()));
	connect(m_addGradientAction, SIGNAL(triggered()), gradientActionMapper, SLOT(map()));
	connect(m_addColorAction, SIGNAL(triggered()), colorActionMapper, SLOT(map()));
	connect(m_addFontAction, SIGNAL(triggered()), this, SLOT(slotAddFont()));

	const char * const imageProperties[] = {
		"background-image",
		"border-image",
		"image",
		0
	};

	const char * const colorProperties[] = {
		"color",
		"background-color",
		"alternate-background-color",
		"border-color",
		"border-top-color",
		"border-right-color",
		"border-bottom-color",
		"border-left-color",
		"gridline-color",
		"selection-color",
		"selection-background-color",
		0
	};

	QMenu *imageActionMenu = new QMenu(this);
	QMenu *gradientActionMenu = new QMenu(this);
	QMenu *colorActionMenu = new QMenu(this);

	ui->statusBarPreview->showMessage("Status Bar");
	ui->statusBarPreview->addPermanentWidget(new QLabel(tr("Status Bar Item"), this));

	for (int imageProperty = 0; imageProperties[imageProperty]; ++imageProperty) {
		QAction *action = imageActionMenu->addAction(QLatin1String(imageProperties[imageProperty]));
		connect(action, SIGNAL(triggered()), imageActionMapper, SLOT(map()));
		imageActionMapper->setMapping(action, QLatin1String(imageProperties[imageProperty]));
	}

	for (int colorProperty = 0; colorProperties[colorProperty]; ++colorProperty) {
		QAction *gradientAction = gradientActionMenu->addAction(QLatin1String(colorProperties[colorProperty]));
		QAction *colorAction = colorActionMenu->addAction(QLatin1String(colorProperties[colorProperty]));
		connect(gradientAction, SIGNAL(triggered()), gradientActionMapper, SLOT(map()));
		connect(colorAction, SIGNAL(triggered()), colorActionMapper, SLOT(map()));
		gradientActionMapper->setMapping(gradientAction, QLatin1String(colorProperties[colorProperty]));
		colorActionMapper->setMapping(colorAction, QLatin1String(colorProperties[colorProperty]));
	}

	connect(imageActionMapper, SIGNAL(mapped(QString)), this, SLOT(slotAddImage(QString)));
	connect(gradientActionMapper, SIGNAL(mapped(QString)), this, SLOT(slotAddGradient(QString)));
	connect(colorActionMapper, SIGNAL(mapped(QString)), this, SLOT(slotAddColor(QString)));

	m_addImageAction->setMenu(imageActionMenu);
	m_addGradientAction->setMenu(gradientActionMenu);
	m_addColorAction->setMenu(colorActionMenu);

	ui->toolBarStyleSheetEditor->addAction(m_addImageAction);
	ui->toolBarStyleSheetEditor->addAction(m_addGradientAction);
	ui->toolBarStyleSheetEditor->addAction(m_addColorAction);
	ui->toolBarStyleSheetEditor->addAction(m_addFontAction);

	ui->plainTextEditStyleSheet->setFocus();
	ui->plainTextEditStyleSheet->setTabStopWidth(fontMetrics().width(QLatin1Char(' '))*4);
	new CssHighlighter(ui->plainTextEditStyleSheet->document());
	connect(ui->plainTextEditStyleSheet, SIGNAL(textChanged()), this, SLOT(validateStyleSheet()));

	gradientManager = new QtGradientManager(this);
	saved = true;
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
	QMainWindow::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void MainWindow::closeEvent(QCloseEvent *e)
{
	Q_UNUSED(e);

	if (!saved && !skinSettings.skinName.isEmpty())
	{
		QMessageBox *msgBox = new QMessageBox(QMessageBox::Question, tr("Skin Not Saved"), tr("The skin has not been saved. Would you like to save it now?"), QMessageBox::Ok|QMessageBox::Cancel, this);
		bool ok = msgBox->exec();
		if (ok)
			on_actionSave_triggered();
	}
}

void MainWindow::on_actionNew_triggered()
{
	this->on_actionClose_triggered();

	DialogNewSkin *dlgNewSkin = new DialogNewSkin(this);
	bool ok = dlgNewSkin->exec();

	if (ok && !(dlgNewSkin->name.isEmpty() && dlgNewSkin->author.isEmpty()))
	{

		enableEditing(true);
		setWindowTitle(dlgNewSkin->name + ".qsf" + tr(" - Quazaa Skin Tool"));

		QDir skinPath = QString(qApp->applicationDirPath() + "/Skin/" + dlgNewSkin->name + "/");
		skinSettings.loadSkin(skinPath.canonicalPath() + dlgNewSkin->name + ".qsf");

		skinSettings.skinName = dlgNewSkin->name;

		ui->lineEditName->setText(skinSettings.skinName);
		ui->lineEditAuthor->setText(dlgNewSkin->author);
		skinSettings.skinAuthor = dlgNewSkin->author;
		ui->lineEditVersion->setText(dlgNewSkin->version);
		skinSettings.skinVersion = dlgNewSkin->version;
		ui->plainTextEditDescription->setPlainText(dlgNewSkin->description);
		skinSettings.skinDescription = dlgNewSkin->description;

		skinChangeEvent();
		saved = false;
	}
}

void MainWindow::slotContextMenuRequested()
{
	QMenu *menu = ui->plainTextEditStyleSheet->createStandardContextMenu();
	menu->addSeparator();
	menu->addAction(m_addImageAction);
	menu->addAction(m_addGradientAction);
	menu->exec(QCursor::pos());
	delete menu;
}

void MainWindow::validateStyleSheet()
{
	const bool valid = isStyleSheetValid(ui->plainTextEditStyleSheet->toPlainText());
	if (valid) {
		ui->labelValidStyleSheet->setText(tr("Valid Style Sheet"));
		ui->labelValidStyleSheet->setStyleSheet(QLatin1String("margin-left: 2px; color: green;"));
	} else {
		ui->labelValidStyleSheet->setText(tr("Invalid Style Sheet"));
		ui->labelValidStyleSheet->setStyleSheet(QLatin1String("margin-left: 2px; color: red;"));
	}
}

bool MainWindow::isStyleSheetValid(const QString &styleSheet)
{
	QuazaaCss::CssParser parser(styleSheet);
	QuazaaCss::CssStyleSheet sheet;
	if (parser.parse(&sheet))
		return true;
	QString fullSheet = QLatin1String("* { ");
	fullSheet += styleSheet;
	fullSheet += QLatin1Char('}');
	QuazaaCss::CssParser parser2(fullSheet);
	return parser2.parse(&sheet);
}

void MainWindow::slotAddImage(const QString &property)
{
	QString path = "";
	QString copyPath = QFileDialog::getOpenFileName(this, tr("Open Image"),
													QString(qApp->applicationDirPath() + "/Skin/" + skinSettings.skinName + "/"),
														   tr("Images (*.png *.xpm *.jpg *.gif *.bmp)"));

	if (!copyPath.isEmpty())
	{
		QString destinationPath = "";
		QDir skinPath = QString(qApp->applicationDirPath() + "/Skin/" + skinSettings.skinName + "/");
		if (!skinPath.exists())
		{
			skinPath.mkpath(QString(qApp->applicationDirPath() + "/Skin/" + skinSettings.skinName + "/"));
		}

		if (!copyPath.contains(QString(qApp->applicationDirPath()) + "/Skin/" + skinSettings.skinName + "/"))
		{
			QFileInfo fileInfo(copyPath);
			destinationPath = qApp->applicationDirPath() + "/Skin/" + ui->lineEditName->text() + "/" + fileInfo.fileName();
			destinationPath = destinationPath.replace("//", "/", Qt::CaseInsensitive);
			QFile::copy(copyPath, destinationPath);
		} else {
			destinationPath = copyPath;
			destinationPath = destinationPath.replace("//", "/", Qt::CaseInsensitive);
		}

		path = destinationPath.remove(QApplication::applicationDirPath() + "/");

		if (!path.isEmpty())
			insertCssProperty(property, QString(QLatin1String("url(%1)")).arg(path));
	}
}

void MainWindow::slotAddGradient(const QString &property)
{
	bool ok;
	const QGradient grad = QtGradientViewDialog::getGradient(&ok, gradientManager, this);
	if (ok)
		insertCssProperty(property, QtGradientUtils::styleSheetCode(grad));
}

void MainWindow::slotAddColor(const QString &property)
{
	const QColor color = QColorDialog::getColor(0xffffffff, this, QString(), QColorDialog::ShowAlphaChannel);
	if (!color.isValid())
		return;

	QString colorStr;

	if (color.alpha() == 255) {
		colorStr = QString(QLatin1String("rgb(%1, %2, %3)")).arg(
				color.red()).arg(color.green()).arg(color.blue());
	} else {
		colorStr = QString(QLatin1String("rgba(%1, %2, %3, %4)")).arg(
				color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha());
	}

	insertCssProperty(property, colorStr);
}

void MainWindow::slotAddFont()
{
	bool ok;
	QFont font = QFontDialog::getFont(&ok, this);
	if (ok) {
		QString fontStr;
		if (font.bold()) {
			fontStr += "bold";
			fontStr += QLatin1Char(' ');
		}

		switch (font.style()) {
		case QFont::StyleItalic:
			fontStr += QLatin1String("italic ");
			break;
		case QFont::StyleOblique:
			fontStr += QLatin1String("oblique ");
			break;
		default:
			break;
		}
		fontStr += QString::number(font.pointSize());
		fontStr += QLatin1String("pt \"");
		fontStr += font.family();
		fontStr += QLatin1Char('"');

		insertCssProperty(QLatin1String("font"), fontStr);
		QString decoration;
		if (font.underline())
			decoration += QLatin1String("underline");
		if (font.strikeOut()) {
			if (!decoration.isEmpty())
				decoration += QLatin1Char(' ');
			decoration += QLatin1String("line-through");
		}
		insertCssProperty(QLatin1String("text-decoration"), decoration);
	}
}

void MainWindow::insertCssProperty(const QString &name, const QString &value)
{
	if (!value.isEmpty()) {
		QTextCursor cursor = ui->plainTextEditStyleSheet->textCursor();
		if (!name.isEmpty()) {
			cursor.beginEditBlock();
			cursor.removeSelectedText();
			cursor.movePosition(QTextCursor::EndOfLine);

			// Simple check to see if we're in a selector scope
			const QTextDocument *doc = ui->plainTextEditStyleSheet->document();
			const QTextCursor closing = doc->find(QLatin1String("}"), cursor, QTextDocument::FindBackward);
			const QTextCursor opening = doc->find(QLatin1String("{"), cursor, QTextDocument::FindBackward);
			const bool inSelector = !opening.isNull() && (closing.isNull() ||
														  closing.position() < opening.position());
			QString insertion;
			if (ui->plainTextEditStyleSheet->textCursor().block().length() != 1)
				insertion += QLatin1Char('\n');
			if (inSelector)
				insertion += QLatin1Char('\t');
			insertion += name;
			insertion += QLatin1String(": ");
			insertion += value;
			insertion += QLatin1Char(';');
			cursor.insertText(insertion);
			cursor.endEditBlock();
		} else {
			cursor.insertText(value);
		}
	}
}

void MainWindow::enableEditing(bool enable)
{
	m_addImageAction->setEnabled(enable);
	m_addGradientAction->setEnabled(enable);
	m_addColorAction->setEnabled(enable);
	m_addFontAction->setEnabled(enable);
	ui->lineEditVersion->setEnabled(enable);
	ui->plainTextEditDescription->setEnabled(enable);
	ui->checkBoxMainIconVisible->setEnabled(enable);
}


void MainWindow::on_treeWidgetSelector_itemClicked(QTreeWidgetItem* item, int column)
{
	if (item->isSelected())
	{
		bool tempSaved = saved;
		currentSelectionText = item->text(column);
		if (!ui->lineEditName->text().isEmpty() && !ui->lineEditAuthor->text().isEmpty() && ui->treeWidgetSelector->currentIndex().isValid()) {
			ui->plainTextEditStyleSheet->setEnabled(true);
		}

		if (item->text(column) == tr("Skin Properties"))
		{
			ui->stackedWidget->setCurrentIndex(0);
			ui->plainTextEditStyleSheet->setPlainText("");
			ui->plainTextEditStyleSheet->setEnabled(false);
		}

		if (item->text(column) == tr("Splash Screen Background"))
		{
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.splashBackground);
			ui->stackedWidget->setCurrentIndex(1);
		}

		if (item->text(column) == tr("Logo"))
		{
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.splashLogo);
			ui->stackedWidget->setCurrentIndex(1);
		}

		if (item->text(column) == tr("Footer"))
		{
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.splashFooter);
			ui->stackedWidget->setCurrentIndex(1);
		}

		if (item->text(column) == tr("Status Text"))
		{
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.splashStatus);
			ui->stackedWidget->setCurrentIndex(1);
		}

		if (item->text(column) == tr("Progress"))
		{
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.splashProgress);
			ui->stackedWidget->setCurrentIndex(1);
		}

		if (item->text(column) == tr("Top Left"))
		{
			ui->windowText->setText(tr("Main Window"));
			this->isMainWindow = true;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.windowFrameTopLeftStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == tr("Left"))
		{
			ui->windowText->setText(tr("Main Window"));
			this->isMainWindow = true;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.windowFrameLeftStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == "Bottom Left")
		{
			ui->windowText->setText(tr("Main Window"));
			this->isMainWindow = true;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.windowFrameBottomLeftStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == tr("Top"))
		{
			ui->windowText->setText(tr("Main Window"));
			this->isMainWindow = true;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.windowFrameTopStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == tr("Icon Frame"))
		{
			ui->windowText->setText(tr("Main Window"));
			this->isMainWindow = true;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.windowIconFrameStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == tr("Window Text"))
		{
			ui->windowText->setText(tr("Main Window"));
			this->isMainWindow = true;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.windowTextStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == tr("Spacer Frame"))
		{
			ui->windowText->setText(tr("Main Window"));
			this->isMainWindow = true;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.windowFrameTopSpacerStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == tr("Buttons Frame"))
		{
			ui->windowText->setText(tr("Main Window"));
			this->isMainWindow = true;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.titlebarButtonsFrameStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == tr("Minimize Button"))
		{
			ui->windowText->setText(tr("Main Window"));
			this->isMainWindow = true;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.minimizeButtonStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == tr("Maximize Button"))
		{
			ui->windowText->setText(tr("Main Window"));
			this->isMainWindow = true;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.maximizeButtonStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == tr("Close Button"))
		{
			ui->windowText->setText(tr("Main Window"));
			this->isMainWindow = true;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.closeButtonStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == tr("Bottom"))
		{
			ui->windowText->setText(tr("Main Window"));
			this->isMainWindow = true;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.windowFrameBottomStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == tr("Top Right"))
		{
			ui->windowText->setText(tr("Main Window"));
			this->isMainWindow = true;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.windowFrameTopRightStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == tr("Right"))
		{
			ui->windowText->setText(tr("Main Window"));
			this->isMainWindow = true;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.windowFrameRightStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == tr("Bottom Right"))
		{
			ui->windowText->setText(tr("Main Window"));
			this->isMainWindow = true;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.windowFrameBottomRightStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == tr("Child Top Left"))
		{
			ui->windowText->setText(tr("Child Window"));
			this->isMainWindow = false;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.childWindowFrameTopLeftStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == tr("Child Left"))
		{
			ui->windowText->setText(tr("Child Window"));
			this->isMainWindow = false;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.childWindowFrameLeftStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == tr("Child Bottom Left"))
		{
			ui->windowText->setText(tr("Child Window"));
			this->isMainWindow = false;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.childWindowFrameBottomLeftStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == tr("Child Top"))
		{
			ui->windowText->setText(tr("Child Window"));
			this->isMainWindow = false;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.childWindowFrameTopStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == tr("Child Icon Frame"))
		{
			ui->windowText->setText(tr("Child Window"));
			this->isMainWindow = false;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.childWindowIconFrameStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == tr("Child Window Text"))
		{
			ui->windowText->setText(tr("Child Window"));
			this->isMainWindow = false;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.childWindowTextStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == tr("Child Spacer Frame"))
		{
			ui->windowText->setText(tr("Child Window"));
			this->isMainWindow = false;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.childWindowFrameTopSpacerStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == tr("Child Buttons Frame"))
		{
			ui->windowText->setText(tr("Child Window"));
			this->isMainWindow = false;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.childTitlebarButtonsFrameStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == tr("Child Minimize Button"))
		{
			ui->windowText->setText(tr("Child Window"));
			this->isMainWindow = false;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.childMinimizeButtonStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == tr("Child Maximize Button"))
		{
			ui->windowText->setText(tr("Child Window"));
			this->isMainWindow = false;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.childMaximizeButtonStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == tr("Child Close Button"))
		{
			ui->windowText->setText(tr("Child Window"));
			this->isMainWindow = false;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.childCloseButtonStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == tr("Child Bottom"))
		{
			ui->windowText->setText(tr("Child Window"));
			this->isMainWindow = false;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.childWindowFrameBottomStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == tr("Child Top Right"))
		{
			ui->windowText->setText(tr("Child Window"));
			this->isMainWindow = false;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.childWindowFrameTopRightStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == tr("Child Right"))
		{
			ui->windowText->setText(tr("Child Window"));
			this->isMainWindow = false;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.childWindowFrameRightStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == tr("Child Bottom Right"))
		{
			ui->windowText->setText(tr("Child Window"));
			this->isMainWindow = false;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.childWindowFrameBottomRightStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == tr("Standard Items"))
		{
			ui->stackedWidget->setCurrentIndex(3);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.standardItems);
		}

		if (item->text(column) == tr("List Views"))
		{
			ui->stackedWidget->setCurrentIndex(4);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.listViews);
		}

		if (item->text(column) == tr("Main Menu Toolbar"))
		{
			ui->stackedWidget->setCurrentIndex(5);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.mainMenuToolbar);
		}

		if (item->text(column) == tr("Sidebar Background"))
		{
			ui->stackedWidget->setCurrentIndex(5);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.sidebarBackground);
		}

		if (item->text(column) == tr("Sidebar Task Header"))
		{
			ui->stackedWidget->setCurrentIndex(5);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.sidebarTaskHeader);
		}

		if (item->text(column) == tr("Sidebar Task Body"))
		{
			ui->stackedWidget->setCurrentIndex(5);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.sidebarTaskBackground);
		}

		if (item->text(column) == tr("Un-Clickable Sidebar Task Header"))
		{
			ui->stackedWidget->setCurrentIndex(5);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.sidebarUnclickableTaskHeader);
		}

		if (item->text(column) == tr("Add Search Button"))
		{
			ui->stackedWidget->setCurrentIndex(5);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.addSearchButton);
		}

		if (item->text(column) == tr("Chat Welcome"))
		{
			ui->stackedWidget->setCurrentIndex(5);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.chatWelcome);
		}

		if (item->text(column) == tr("Chat Toolbar"))
		{
			ui->stackedWidget->setCurrentIndex(5);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.chatToolbar);
		}

		if (item->text(column) == tr("Library Navigator"))
		{
			ui->stackedWidget->setCurrentIndex(5);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.libraryNavigator);
		}

		if (currentSelectionText == tr("Library View Header"))
		{
			ui->stackedWidget->setCurrentIndex(5);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.libraryViewHeader);
		}

		if (item->text(column) == tr("Searches"))
		{
			ui->stackedWidget->setCurrentIndex(5);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.tabSearches);
		}

		if (item->text(column) == tr("Toolbars"))
		{
			ui->stackedWidget->setCurrentIndex(5);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.toolbars);
		}

		if (item->text(column) == tr("Media Toolbar"))
		{
			ui->stackedWidget->setCurrentIndex(5);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.mediaToolbar);
		}

		if (item->text(column) == tr("Media Seek Slider"))
		{
			ui->stackedWidget->setCurrentIndex(5);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.seekSlider);
		}

		if (item->text(column) == tr("Media Volume Slider"))
		{
			ui->stackedWidget->setCurrentIndex(5);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.volumeSlider);
		}

		if (item->text(column) == tr("Toolbar"))
		{
			ui->stackedWidget->setCurrentIndex(6);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.navigationToolbar);
		}

		if (item->text(column) == tr("Home"))
		{
			ui->stackedWidget->setCurrentIndex(6);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.homeHeader);
		}

		if (item->text(column) == tr("Library"))
		{
			ui->stackedWidget->setCurrentIndex(6);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.libraryHeader);
		}

		if (item->text(column) == tr("Media"))
		{
			ui->stackedWidget->setCurrentIndex(6);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.mediaHeader);
		}

		if (item->text(column) == tr("Search"))
		{
			ui->stackedWidget->setCurrentIndex(6);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.searchHeader);
		}

		if (item->text(column) == tr("Transfers"))
		{
			ui->stackedWidget->setCurrentIndex(6);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.transfersHeader);
		}

		if (item->text(column) == tr("Security"))
		{
			ui->stackedWidget->setCurrentIndex(6);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.securityHeader);
		}

		if (item->text(column) == tr("Activity"))
		{
			ui->stackedWidget->setCurrentIndex(6);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.activityHeader);
		}

		if (item->text(column) == tr("Chat"))
		{
			ui->stackedWidget->setCurrentIndex(6);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.chatHeader);
		}

		if (item->text(column) == tr("Generic"))
		{
			ui->stackedWidget->setCurrentIndex(6);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.genericHeader);
		}

		if (item->text(column) == tr("Dialog Header"))
		{
			ui->stackedWidget->setCurrentIndex(6);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.dialogHeader);
		}

		if (item->text(column) == tr("Colors"))
		{
			ui->stackedWidget->setCurrentIndex(7);
			ui->plainTextEditStyleSheet->setPlainText("");
			ui->plainTextEditStyleSheet->setEnabled(false);
		}

		qApp->processEvents();
		saved = tempSaved;
	}
}

void MainWindow::on_actionOpen_triggered()
{
	on_actionClose_triggered();

	DialogOpenSkin *dlgOpenSkin = new DialogOpenSkin(this);
	bool ok;
	ok = dlgOpenSkin->exec();
	if (ok)
	{
		skinSettings.loadSkin(dlgOpenSkin->skinFile);
		ui->lineEditName->setText(skinSettings.skinName);
		ui->lineEditAuthor->setText(skinSettings.skinAuthor);
		ui->lineEditVersion->setText(skinSettings.skinVersion);
		ui->plainTextEditDescription->setPlainText(skinSettings.skinDescription);
		ui->toolButtonColorInformation->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + skinSettings.logColorInformation.name() + ";}");
		ui->toolButtonColorSecurity->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + skinSettings.logColorSecurity.name() + ";}");
		ui->toolButtonColorNotice->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + skinSettings.logColorNotice.name() + ";}");
		ui->toolButtonColorDebug->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + skinSettings.logColorDebug.name() + ";}");
		ui->toolButtonColorWarning->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + skinSettings.logColorWarning.name() + ";}");
		ui->toolButtonColorError->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + skinSettings.logColorError.name() + ";}");
		ui->toolButtonColorCritical->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + skinSettings.logColorCritical.name() + ";}");
		ui->toolButtonColorListsActive->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + skinSettings.listsColorActive.name() + ";}");
		ui->toolButtonColorListsHighlighted->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + skinSettings.listsColorHighlighted.name() + ";}");
		ui->toolButtonColorListsNormal->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + skinSettings.listsColorNormal.name() + ";}");
		ui->toolButtonColorListsSpecial->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + skinSettings.listsColorSpecial.name() + ";}");
		ui->textEditLogPreview->setStyleSheet(skinSettings.standardItems);
		ui->toolButtonColorMessagesActions->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + skinSettings.chatMessagesColorActions.name() + ";}");
		ui->toolButtonColorMessagesHighlighted->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + skinSettings.chatMessagesColorHighlighted.name() + ";}");
		ui->toolButtonColorMessagesNormal->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + skinSettings.chatMessagesColorNormal.name() + ";}");
		ui->toolButtonColorMessagesNotices->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + skinSettings.chatMessagesColorNotices.name() + ";}");
		ui->toolButtonColorMessagesServer->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + skinSettings.chatMessagesColorServer.name() + ";}");
		ui->toolButtonColorMessagesTopics->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + skinSettings.chatMessagesColorTopics.name() + ";}");
		ui->toolButtonColorUsersAdministrator->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + skinSettings.chatUsersColorAdministrator.name() + ";}");
		ui->toolButtonColorUsersHalfOperator->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + skinSettings.chatUsersColorHalfOperator.name() + ";}");
		ui->toolButtonColorUsersNormal->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + skinSettings.chatUsersColorNormal.name() + ";}");
		ui->toolButtonColorUsersOperator->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + skinSettings.chatUsersColorOperator.name() + ";}");
		ui->toolButtonColorUsersOwner->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + skinSettings.chatUsersColorOwner.name() + ";}");
		ui->toolButtonColorUsersVoiced->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + skinSettings.chatUsersColorVoiced.name() + ";}");

		if (skinSettings.logWeightInformation == "font-weight:600;")
		{
			ui->checkBoxInformationBold->setChecked(true);
		} else {
			ui->checkBoxInformationBold->setChecked(false);
		}

		if (skinSettings.logWeightSecurity == "font-weight:600;")
		{
			ui->checkBoxSecurityBold->setChecked(true);
		} else {
			ui->checkBoxSecurityBold->setChecked(false);
		}

		if (skinSettings.logWeightNotice == "font-weight:600;")
		{
			ui->checkBoxNoticeBold->setChecked(true);
		} else {
			ui->checkBoxNoticeBold->setChecked(false);
		}

		if (skinSettings.logWeightDebug == "font-weight:600;")
		{
			ui->checkBoxDebugBold->setChecked(true);
		} else {
			ui->checkBoxDebugBold->setChecked(false);
		}

		if (skinSettings.logWeightWarning == "font-weight:600;")
		{
			ui->checkBoxWarningBold->setChecked(true);
		} else {
			ui->checkBoxWarningBold->setChecked(false);
		}

		if (skinSettings.logWeightError == "font-weight:600;")
		{
			ui->checkBoxErrorBold->setChecked(true);
		} else {
			ui->checkBoxErrorBold->setChecked(false);
		}

		if (skinSettings.logWeightCritical == "font-weight:600;")
		{
			ui->checkBoxCriticalBold->setChecked(true);
		} else {
			ui->checkBoxCriticalBold->setChecked(false);
		}

		if (skinSettings.listsWeightActive == 75)
		{
			ui->checkBoxBoldListsActive->setChecked(true);
		} else {
			ui->checkBoxBoldListsActive->setChecked(false);
		}

		if (skinSettings.listsWeightHighlighted == 75)
		{
			ui->checkBoxBoldListsHighlighted->setChecked(true);
		} else {
			ui->checkBoxBoldListsHighlighted->setChecked(false);
		}

		if (skinSettings.listsWeightNormal == 75)
		{
			ui->checkBoxBoldListsNormal->setChecked(true);
		} else {
			ui->checkBoxBoldListsNormal->setChecked(false);
		}

		if (skinSettings.listsWeightSpecial == 75)
		{
			ui->checkBoxBoldListsSpecial->setChecked(true);
		} else {
			ui->checkBoxBoldListsSpecial->setChecked(false);
		}

		if (skinSettings.chatMessagesWeightNormal == "font-weight:600;")
		{
			ui->checkBoxBoldMessagesNormal->setChecked(true);
		} else {
			ui->checkBoxBoldMessagesNormal->setChecked(false);
		}

		if (skinSettings.chatMessagesWeightHighlighted == "font-weight:600;")
		{
			ui->checkBoxBoldMessagesHighlighted->setChecked(true);
		} else {
			ui->checkBoxBoldMessagesHighlighted->setChecked(false);
		}

		if (skinSettings.chatMessagesWeightServer == "font-weight:600;")
		{
			ui->checkBoxBoldMessagesServer->setChecked(true);
		} else {
			ui->checkBoxBoldMessagesServer->setChecked(false);
		}
		if (skinSettings.chatMessagesWeightTopics == "font-weight:600;")
		{
			ui->checkBoxBoldMessagesTopics->setChecked(true);
		} else {
			ui->checkBoxBoldMessagesTopics->setChecked(false);
		}
		if (skinSettings.chatMessagesWeightNotices == "font-weight:600;")
		{
			ui->checkBoxBoldMessagesNotices->setChecked(true);
		} else {
			ui->checkBoxBoldMessagesNotices->setChecked(false);
		}
		if (skinSettings.chatMessagesWeightActions == "font-weight:600;")
		{
			ui->checkBoxBoldMessagesActions->setChecked(true);
		} else {
			ui->checkBoxBoldMessagesActions->setChecked(false);
		}

		if (skinSettings.chatUsersWeightNormal == 75)
		{
			ui->checkBoxBoldUsersNormal->setChecked(true);
		} else {
			ui->checkBoxBoldUsersNormal->setChecked(false);
		}

		if (skinSettings.chatUsersWeightOwner == 75)
		{
			ui->checkBoxBoldUsersOwner->setChecked(true);
		} else {
			ui->checkBoxBoldUsersOwner->setChecked(false);
		}

		if (skinSettings.chatUsersWeightAdministrator == 75)
		{
			ui->checkBoxBoldUsersAdministrator->setChecked(true);
		} else {
			ui->checkBoxBoldUsersAdministrator->setChecked(false);
		}

		if (skinSettings.chatUsersWeightOperator == 75)
		{
			ui->checkBoxBoldUsersOperator->setChecked(true);
		} else {
			ui->checkBoxBoldUsersOperator->setChecked(false);
		}

		if (skinSettings.chatUsersWeightHalfOperator == 75)
		{
			ui->checkBoxBoldUsersHalfOperator->setChecked(true);
		} else {
			ui->checkBoxBoldUsersHalfOperator->setChecked(false);
		}

		if (skinSettings.chatUsersWeightVoiced == 75)
		{
			ui->checkBoxBoldUsersVoiced->setChecked(true);
		} else {
			ui->checkBoxBoldUsersVoiced->setChecked(false);
		}

		updateLogPreview();
		skinChangeEvent();
		setWindowTitle(skinSettings.skinName + ".qsf" + " - Quazaa Skin Tool");
		enableEditing(true);
		saved = true;
	}
	on_treeWidgetSelector_itemClicked(ui->treeWidgetSelector->currentItem(), 0);
}

void MainWindow::on_actionClose_triggered()
{
	if (!saved && !skinSettings.skinName.isEmpty())
	{
		QMessageBox *msgBox = new QMessageBox(QMessageBox::Question, tr("Skin Not Saved"), tr("The skin has not been saved. Would you like to save it now?"), QMessageBox::Ok|QMessageBox::Cancel, this);
		int ok = msgBox->exec();
		if (ok == QDialog::Accepted)
		{
			on_actionSave_triggered();
		}
	}

	ui->plainTextEditStyleSheet->setEnabled(false);
	enableEditing(false);
	skinChangeEvent();
	setWindowTitle(tr("Quazaa Skin Tool"));
}

void MainWindow::on_actionSave_triggered()
{
	skinSettings.saveSkin(qApp->applicationDirPath() + "/Skin/" + skinSettings.skinName + "/" + skinSettings.skinName + ".qsk");
	saved = true;
}

void MainWindow::on_actionExit_triggered()
{
	close();
}

void MainWindow::on_actionPackage_For_Distribution_triggered()
{

}

void MainWindow::on_actionCut_triggered()
{

}

void MainWindow::on_actionCopy_triggered()
{

}

void MainWindow::on_actionPaste_triggered()
{

}

void MainWindow::on_actionSkin_Creation_Guide_triggered()
{

}

void MainWindow::on_actionAbout_Quazaa_Skin_Tool_triggered()
{

}

void MainWindow::skinChangeEvent()
{
	ui->frameSplashBackground->setStyleSheet(skinSettings.splashBackground);
	ui->labelSplashlLogo->setStyleSheet(skinSettings.splashLogo);
	ui->frameSplashFooter->setStyleSheet(skinSettings.splashFooter);
	ui->labelSplashStatus->setStyleSheet(skinSettings.splashStatus);
	ui->progressBarSplashStatus->setStyleSheet(skinSettings.splashProgress);
	updateWindowStyleSheet(isMainWindow);
	ui->pageStandardItems->setStyleSheet(skinSettings.standardItems);
	ui->pageLists->setStyleSheet(skinSettings.standardItems);
	ui->treeWidget->setStyleSheet(skinSettings.listViews);
	ui->tableWidget->setStyleSheet(skinSettings.listViews);
	ui->listWidget->setStyleSheet(skinSettings.listViews);
	ui->scrollAreaPreview->setStyleSheet(skinSettings.listViews);
	ui->textEditLogPreview->setStyleSheet(skinSettings.standardItems);
	pageExtendedItems->skinChangeEvent();
	pageNavigation->skinChangeEvent();
}

void MainWindow::updateWindowStyleSheet(bool mainWindow)
{
	if (mainWindow)
	{
		ui->windowFrameTopLeft->setStyleSheet(skinSettings.windowFrameTopLeftStyleSheet);
		ui->windowFrameLeft->setStyleSheet(skinSettings.windowFrameLeftStyleSheet);
		ui->windowFrameBottomLeft->setStyleSheet(skinSettings.windowFrameBottomLeftStyleSheet);
		ui->windowFrameTop->setStyleSheet(skinSettings.windowFrameTopStyleSheet);
		ui->windowIconFrame->setStyleSheet(skinSettings.windowIconFrameStyleSheet);
		ui->checkBoxMainIconVisible->setChecked(skinSettings.windowIconVisible);
		ui->windowIcon->setVisible(skinSettings.windowIconVisible);
		ui->windowText->setStyleSheet(skinSettings.windowTextStyleSheet);
		ui->windowFrameTopSpacer->setStyleSheet(skinSettings.windowFrameTopSpacerStyleSheet);
		ui->titlebarButtonsFrame->setStyleSheet(skinSettings.titlebarButtonsFrameStyleSheet);
		ui->minimizeButton->setStyleSheet(skinSettings.minimizeButtonStyleSheet);
		ui->maximizeButton->setStyleSheet(skinSettings.maximizeButtonStyleSheet);
		ui->closeButton->setStyleSheet(skinSettings.closeButtonStyleSheet);
		ui->windowFrameBottom->setStyleSheet(skinSettings.windowFrameBottomStyleSheet);
		ui->windowFrameTopRight->setStyleSheet(skinSettings.windowFrameTopRightStyleSheet);
		ui->windowFrameRight->setStyleSheet(skinSettings.windowFrameRightStyleSheet);
		ui->windowFrameBottomRight->setStyleSheet(skinSettings.windowFrameBottomRightStyleSheet);
	} else {
		ui->windowFrameTopLeft->setStyleSheet(skinSettings.childWindowFrameTopLeftStyleSheet);
		ui->windowFrameLeft->setStyleSheet(skinSettings.childWindowFrameLeftStyleSheet);
		ui->windowFrameBottomLeft->setStyleSheet(skinSettings.childWindowFrameBottomLeftStyleSheet);
		ui->windowFrameTop->setStyleSheet(skinSettings.childWindowFrameTopStyleSheet);
		ui->windowIconFrame->setStyleSheet(skinSettings.childWindowIconFrameStyleSheet);
		ui->checkBoxMainIconVisible->setChecked(skinSettings.childWindowIconVisible);
		ui->windowIcon->setVisible(skinSettings.childWindowIconVisible);
		ui->windowText->setStyleSheet(skinSettings.childWindowTextStyleSheet);
		ui->windowFrameTopSpacer->setStyleSheet(skinSettings.childWindowFrameTopSpacerStyleSheet);
		ui->titlebarButtonsFrame->setStyleSheet(skinSettings.childTitlebarButtonsFrameStyleSheet);
		ui->minimizeButton->setStyleSheet(skinSettings.childMinimizeButtonStyleSheet);
		ui->maximizeButton->setStyleSheet(skinSettings.childMaximizeButtonStyleSheet);
		ui->closeButton->setStyleSheet(skinSettings.childCloseButtonStyleSheet);
		ui->windowFrameBottom->setStyleSheet(skinSettings.childWindowFrameBottomStyleSheet);
		ui->windowFrameTopRight->setStyleSheet(skinSettings.childWindowFrameTopRightStyleSheet);
		ui->windowFrameRight->setStyleSheet(skinSettings.childWindowFrameRightStyleSheet);
		ui->windowFrameBottomRight->setStyleSheet(skinSettings.childWindowFrameBottomRightStyleSheet);
	}
}

void MainWindow::applySheets()
{
	if (currentSelectionText == tr("Splash Screen Background"))
	{
		skinSettings.splashBackground = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Logo"))
	{
		skinSettings.splashLogo = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Footer"))
	{
		skinSettings.splashFooter = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Status Text"))
	{
		skinSettings.splashStatus = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Progress"))
	{
		skinSettings.splashProgress = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Top Left"))
	{
		skinSettings.windowFrameTopLeftStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Left"))
	{
		skinSettings.windowFrameLeftStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Bottom Left"))
	{
		skinSettings.windowFrameBottomLeftStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Top"))
	{
		skinSettings.windowFrameTopStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Icon Frame"))
	{
		skinSettings.windowIconFrameStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Window Text"))
	{
		skinSettings.windowTextStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Spacer Frame"))
	{
		skinSettings.windowFrameTopSpacerStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Buttons Frame"))
	{
		skinSettings.titlebarButtonsFrameStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Minimize Button"))
	{
		skinSettings.minimizeButtonStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Maximize Button"))
	{
		skinSettings.maximizeButtonStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Close Button"))
	{
		skinSettings.closeButtonStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Bottom"))
	{
		skinSettings.windowFrameBottomStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Top Right"))
	{
		skinSettings.windowFrameTopRightStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Right"))
	{
		skinSettings.windowFrameRightStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Bottom Right"))
	{
		skinSettings.windowFrameBottomRightStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Child Top Left"))
	{
		skinSettings.childWindowFrameTopLeftStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Child Left"))
	{
		skinSettings.childWindowFrameLeftStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Child Bottom Left"))
	{
		skinSettings.childWindowFrameBottomLeftStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Child Top"))
	{
		skinSettings.childWindowFrameTopStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Child Icon Frame"))
	{
		skinSettings.childWindowIconFrameStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Child Window Text"))
	{
		skinSettings.childWindowTextStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Child Spacer Frame"))
	{
		skinSettings.childWindowFrameTopSpacerStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Child Buttons Frame"))
	{
		skinSettings.childTitlebarButtonsFrameStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Child Minimize Button"))
	{
		skinSettings.childMinimizeButtonStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Child Maximize Button"))
	{
		skinSettings.childMaximizeButtonStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Child Close Button"))
	{
		skinSettings.childCloseButtonStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Child Bottom"))
	{
		skinSettings.childWindowFrameBottomStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Child Top Right"))
	{
		skinSettings.childWindowFrameTopRightStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Child Right"))
	{
		skinSettings.childWindowFrameRightStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Child Bottom Right"))
	{
		skinSettings.childWindowFrameBottomRightStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Standard Items"))
	{
		skinSettings.standardItems = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("List Views"))
	{
		skinSettings.listViews = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Main Menu Toolbar"))
	{
		skinSettings.mainMenuToolbar = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Sidebar Background"))
	{
		skinSettings.sidebarBackground = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Sidebar Task Header"))
	{
		skinSettings.sidebarTaskHeader = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Sidebar Task Body"))
	{
		skinSettings.sidebarTaskBackground = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Un-Clickable Sidebar Task Header"))
	{
		skinSettings.sidebarUnclickableTaskHeader = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Add Search Button"))
	{
		skinSettings.addSearchButton = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Chat Welcome"))
	{
		skinSettings.chatWelcome = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Chat Toolbar"))
	{
		skinSettings.chatToolbar = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Library Navigator"))
	{
		skinSettings.libraryNavigator = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Library View Header"))
	{
		skinSettings.libraryViewHeader = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Searches"))
	{
		skinSettings.tabSearches = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Toolbars"))
	{
		skinSettings.toolbars = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Media Toolbar"))
	{
		skinSettings.mediaToolbar = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Media Seek Slider"))
	{
		skinSettings.seekSlider = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Media Volume Slider"))
	{
		skinSettings.volumeSlider = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Toolbar"))
	{
		skinSettings.navigationToolbar = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Home"))
	{
		skinSettings.homeHeader = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Library"))
	{
		skinSettings.libraryHeader = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Media"))
	{
		skinSettings.mediaHeader = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Search"))
	{
		skinSettings.searchHeader = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Transfers"))
	{
		skinSettings.transfersHeader = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Security"))
	{
		skinSettings.securityHeader = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Activity"))
	{
		skinSettings.activityHeader = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Chat"))
	{
		skinSettings.chatHeader = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Generic"))
	{
		skinSettings.genericHeader = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == tr("Dialog Header"))
	{
		skinSettings.dialogHeader = ui->plainTextEditStyleSheet->toPlainText();
	}
	skinChangeEvent();
}

void MainWindow::on_plainTextEditStyleSheet_textChanged()
{
	if (isStyleSheetValid(ui->plainTextEditStyleSheet->toPlainText()))
	{
		saved = false;
		applySheets();
	}
}

void MainWindow::on_checkBoxMainIconVisible_toggled(bool checked)
{
	Q_UNUSED(checked);

	saved = false;
	applyIcon();
}

void MainWindow::on_lineEditVersion_textChanged(QString text)
{
	Q_UNUSED(text);

	skinSettings.skinVersion = ui->lineEditVersion->text();
	saved = false;
}

void MainWindow::on_plainTextEditDescription_textChanged()
{
	skinSettings.skinDescription = ui->plainTextEditDescription->toPlainText();
	saved = false;
}

void MainWindow::applyIcon()
{
	if (isMainWindow)
	{
		skinSettings.windowIconVisible = ui->checkBoxMainIconVisible->isChecked();
	} else {
		skinSettings.childWindowIconVisible = ui->checkBoxMainIconVisible->isChecked();
	}
	skinChangeEvent();
}

void MainWindow::updateLogPreview()
{
	ui->textEditLogPreview->clear();
	ui->textEditLogPreview->append(QString("<span style=\" font-size:8pt; %1 color:%2;\">%3</span>").arg(skinSettings.logWeightInformation).arg(skinSettings.logColorInformation.name()).arg("Information Message"));
	ui->textEditLogPreview->append(QString("<span style=\" font-size:8pt; %1 color:%2;\">%3:</span>").arg(skinSettings.logWeightSecurity).arg(skinSettings.logColorSecurity.name()).arg("Security Message"));
	ui->textEditLogPreview->append(QString("<span style=\" font-size:8pt; %1 color:%2;\">%3:</span>").arg(skinSettings.logWeightNotice).arg(skinSettings.logColorNotice.name()).arg("Notice Message"));
	ui->textEditLogPreview->append(QString("<span style=\" font-size:8pt; %1 color:%2;\">%3:</span>").arg(skinSettings.logWeightDebug).arg(skinSettings.logColorDebug.name()).arg("Debug Message"));
	ui->textEditLogPreview->append(QString("<span style=\" font-size:8pt; %1 color:%2;\">%3:</span>").arg(skinSettings.logWeightWarning).arg(skinSettings.logColorWarning.name()).arg("Warning Message"));
	ui->textEditLogPreview->append(QString("<span style=\" font-size:8pt; %1 color:%2;\">%3</span>").arg(skinSettings.logWeightError).arg(skinSettings.logColorError.name()).arg("Error Message"));
	ui->textEditLogPreview->append(QString("<span style=\" font-size:8pt; %1 color:%2;\">%3</span>").arg(skinSettings.logWeightCritical).arg(skinSettings.logColorCritical.name()).arg("Critical Error Message"));
}

void MainWindow::on_toolButtonColorInformation_clicked()
{
	QColor color = QColorDialog::getColor(skinSettings.logColorInformation, this);
	if (color.isValid())
	{
		skinSettings.logColorInformation.setNamedColor(color.name());
		ui->toolButtonColorInformation->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + skinSettings.logColorInformation.name() + ";}");
		updateLogPreview();
		saved = false;
	}
}

void MainWindow::on_toolButtonColorSecurity_clicked()
{
	QColor color = QColorDialog::getColor(skinSettings.logColorSecurity, this);
	if (color.isValid())
	{
		skinSettings.logColorSecurity.setNamedColor(color.name());
		ui->toolButtonColorSecurity->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + skinSettings.logColorSecurity.name() + ";}");
		updateLogPreview();
		saved = false;
	}
}

void MainWindow::on_toolButtonColorNotice_clicked()
{
	QColor color = QColorDialog::getColor(skinSettings.logColorNotice, this);
	if (color.isValid())
	{
		skinSettings.logColorNotice.setNamedColor(color.name());
		ui->toolButtonColorNotice->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + skinSettings.logColorNotice.name() + ";}");
		updateLogPreview();
		saved = false;
	}

}

void MainWindow::on_toolButtonColorDebug_clicked()
{
	QColor color = QColorDialog::getColor(skinSettings.logColorDebug, this);
	if (color.isValid())
	{
		skinSettings.logColorDebug.setNamedColor(color.name());
		ui->toolButtonColorDebug->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + skinSettings.logColorDebug.name() + ";}");
		updateLogPreview();
		saved = false;
	}

}

void MainWindow::on_toolButtonColorWarning_clicked()
{
	QColor color = QColorDialog::getColor(skinSettings.logColorWarning, this);
	if (color.isValid())
	{
		skinSettings.logColorWarning.setNamedColor(color.name());
		ui->toolButtonColorWarning->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + skinSettings.logColorWarning.name() + ";}");
		updateLogPreview();
		saved = false;
	}
}

void MainWindow::on_toolButtonColorError_clicked()
{
	QColor color = QColorDialog::getColor(skinSettings.logColorError, this);
	if (color.isValid())
	{
		skinSettings.logColorError.setNamedColor(color.name());
		ui->toolButtonColorError->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + skinSettings.logColorError.name() + ";}");
		updateLogPreview();
		saved = false;
	}
}

void MainWindow::on_toolButtonColorCritical_clicked()
{
	QColor color = QColorDialog::getColor(skinSettings.logColorCritical, this);
	if (color.isValid())
	{
		skinSettings.logColorCritical.setNamedColor(color.name());
		ui->toolButtonColorCritical->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + skinSettings.logColorCritical.name() + ";}");
		updateLogPreview();
		saved = false;
	}
}

void MainWindow::on_checkBoxInformationBold_clicked(bool checked)
{
	if (checked)
	{
		skinSettings.logWeightInformation = "font-weight:600;";
		updateLogPreview();
	} else {
		skinSettings.logWeightInformation = "";
		updateLogPreview();
	}
}

void MainWindow::on_checkBoxSecurityBold_clicked(bool checked)
{
	if (checked)
	{
		skinSettings.logWeightSecurity = "font-weight:600;";
		updateLogPreview();
	} else {
		skinSettings.logWeightSecurity = "";
		updateLogPreview();
	}
}

void MainWindow::on_checkBoxNoticeBold_clicked(bool checked)
{
	if (checked)
	{
		skinSettings.logWeightNotice = "font-weight:600;";
		updateLogPreview();
	} else {
		skinSettings.logWeightNotice = "";
		updateLogPreview();
	}
}

void MainWindow::on_checkBoxDebugBold_clicked(bool checked)
{
	if (checked)
	{
		skinSettings.logWeightDebug = "font-weight:600;";
		updateLogPreview();
	} else {
		skinSettings.logWeightDebug = "";
		updateLogPreview();
	}
}

void MainWindow::on_checkBoxWarningBold_clicked(bool checked)
{
	if (checked)
	{
		skinSettings.logWeightWarning = "font-weight:600;";
		updateLogPreview();
	} else {
		skinSettings.logWeightWarning = "";
		updateLogPreview();
	}
}

void MainWindow::on_checkBoxErrorBold_clicked(bool checked)
{
	if (checked)
	{
		skinSettings.logWeightError = "font-weight:600;";
		updateLogPreview();
	} else {
		skinSettings.logWeightError = "";
		updateLogPreview();
	}
}

void MainWindow::on_checkBoxCriticalBold_clicked(bool checked)
{
	if (checked)
	{
		skinSettings.logWeightCritical = "font-weight:600;";
		updateLogPreview();
	} else {
		skinSettings.logWeightCritical = "";
		updateLogPreview();
	}
}

void MainWindow::on_toolButtonColorListsNormal_clicked()
{
	QColor color = QColorDialog::getColor(skinSettings.listsColorNormal, this);
	if (color.isValid())
	{
			skinSettings.listsColorNormal.setNamedColor(color.name());
			ui->toolButtonColorListsNormal->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + skinSettings.listsColorNormal.name() + ";}");
			updateLogPreview();
			saved = false;
	}
}

void MainWindow::on_checkBoxBoldListsNormal_clicked(bool checked)
{
	if (checked)
	{
		skinSettings.listsWeightNormal = 75;
		updateLogPreview();
	} else {
		skinSettings.listsWeightNormal = 50;
		updateLogPreview();
	}
}

void MainWindow::on_toolButtonColorListsActive_clicked()
{
	QColor color = QColorDialog::getColor(skinSettings.listsColorActive, this);
	if (color.isValid())
	{
			skinSettings.listsColorActive.setNamedColor(color.name());
			ui->toolButtonColorListsActive->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + skinSettings.listsColorActive.name() + ";}");
			updateLogPreview();
			saved = false;
	}
}

void MainWindow::on_checkBoxBoldListsActive_clicked(bool checked)
{
	if (checked)
	{
		skinSettings.listsWeightActive = 75;
		updateLogPreview();
	} else {
		skinSettings.listsWeightActive = 50;
		updateLogPreview();
	}
}

void MainWindow::on_toolButtonColorListsSpecial_clicked()
{
	QColor color = QColorDialog::getColor(skinSettings.listsColorSpecial, this);
	if (color.isValid())
	{
			skinSettings.listsColorSpecial.setNamedColor(color.name());
			ui->toolButtonColorListsSpecial->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + skinSettings.listsColorSpecial.name() + ";}");
			updateLogPreview();
			saved = false;
	}
}

void MainWindow::on_checkBoxBoldListsSpecial_clicked(bool checked)
{
	if (checked)
	{
		skinSettings.listsWeightSpecial = 75;
		updateLogPreview();
	} else {
		skinSettings.listsWeightSpecial = 50;
		updateLogPreview();
	}
}

void MainWindow::on_toolButtonColorListsHighlighted_clicked()
{
	QColor color = QColorDialog::getColor(skinSettings.listsColorHighlighted, this);
	if (color.isValid())
	{
			skinSettings.listsColorHighlighted.setNamedColor(color.name());
			ui->toolButtonColorListsHighlighted->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + skinSettings.listsColorHighlighted.name() + ";}");
			saved = false;
	}
}

void MainWindow::on_checkBoxBoldListsHighlighted_clicked(bool checked)
{
	if (checked)
	{
		skinSettings.listsWeightHighlighted = 75;
		updateLogPreview();
	} else {
		skinSettings.listsWeightHighlighted = 50;
		updateLogPreview();
	}
}

void MainWindow::on_toolButtonColorMessagesNormal_clicked()
{
	QColor color = QColorDialog::getColor(skinSettings.chatMessagesColorNormal, this);
	if (color.isValid())
	{
			skinSettings.chatMessagesColorNormal.setNamedColor(color.name());
			ui->toolButtonColorMessagesNormal->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + skinSettings.chatMessagesColorNormal.name() + ";}");
			saved = false;
	}
}

void MainWindow::on_checkBoxBoldMessagesNormal_clicked(bool checked)
{
	if (checked)
	{
		skinSettings.chatMessagesWeightNormal = "font-weight:600;";
	} else {
		skinSettings.chatMessagesWeightNormal = "";
	}
}

void MainWindow::on_toolButtonColorMessagesHighlighted_clicked()
{
	QColor color = QColorDialog::getColor(skinSettings.chatMessagesColorHighlighted, this);
	if (color.isValid())
	{
			skinSettings.chatMessagesColorHighlighted.setNamedColor(color.name());
			ui->toolButtonColorMessagesHighlighted->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + skinSettings.chatMessagesColorHighlighted.name() + ";}");
			saved = false;
	}
}

void MainWindow::on_checkBoxBoldMessagesHighlighted_clicked(bool checked)
{
	if (checked)
	{
		skinSettings.chatMessagesWeightHighlighted = "font-weight:600;";
	} else {
		skinSettings.chatMessagesWeightHighlighted = "";
	}
}

void MainWindow::on_toolButtonColorMessagesServer_clicked()
{
	QColor color = QColorDialog::getColor(skinSettings.chatMessagesColorServer, this);
	if (color.isValid())
	{
			skinSettings.chatMessagesColorServer.setNamedColor(color.name());
			ui->toolButtonColorMessagesServer->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + skinSettings.chatMessagesColorServer.name() + ";}");
			saved = false;
	}
}

void MainWindow::on_checkBoxBoldMessagesServer_clicked(bool checked)
{
	if (checked)
	{
		skinSettings.chatMessagesWeightServer = "font-weight:600;";
	} else {
		skinSettings.chatMessagesWeightServer = "";
	}
}

void MainWindow::on_toolButtonColorMessagesTopics_clicked()
{
	QColor color = QColorDialog::getColor(skinSettings.chatMessagesColorTopics, this);
	if (color.isValid())
	{
			skinSettings.chatMessagesColorTopics.setNamedColor(color.name());
			ui->toolButtonColorMessagesTopics->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + skinSettings.chatMessagesColorTopics.name() + ";}");
			saved = false;
	}
}

void MainWindow::on_checkBoxBoldMessagesTopics_clicked(bool checked)
{
	if (checked)
	{
		skinSettings.chatMessagesWeightTopics = "font-weight:600;";
	} else {
		skinSettings.chatMessagesWeightTopics = "";
	}
}

void MainWindow::on_toolButtonColorMessagesNotices_clicked()
{
	QColor color = QColorDialog::getColor(skinSettings.chatMessagesColorNotices, this);
	if (color.isValid())
	{
			skinSettings.chatMessagesColorNotices.setNamedColor(color.name());
			ui->toolButtonColorMessagesNotices->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + skinSettings.chatMessagesColorNotices.name() + ";}");
			saved = false;
	}
}

void MainWindow::on_checkBoxBoldMessagesNotices_clicked(bool checked)
{
	if (checked)
	{
		skinSettings.chatMessagesWeightNotices = "font-weight:600;";
	} else {
		skinSettings.chatMessagesWeightNotices = "";
	}
}

void MainWindow::on_toolButtonColorMessagesActions_clicked()
{
	QColor color = QColorDialog::getColor(skinSettings.chatMessagesColorActions, this);
	if (color.isValid())
	{
			skinSettings.chatMessagesColorActions.setNamedColor(color.name());
			ui->toolButtonColorMessagesActions->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + skinSettings.chatMessagesColorActions.name() + ";}");
			saved = false;
	}
}

void MainWindow::on_checkBoxBoldMessagesActions_clicked(bool checked)
{
	if (checked)
	{
		skinSettings.chatMessagesWeightActions = "font-weight:600;";
	} else {
		skinSettings.chatMessagesWeightActions = "";
	}
}

void MainWindow::on_toolButtonColorUsersNormal_clicked()
{
	QColor color = QColorDialog::getColor(skinSettings.chatUsersColorNormal, this);
	if (color.isValid())
	{
			skinSettings.chatUsersColorNormal.setNamedColor(color.name());
			ui->toolButtonColorUsersNormal->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + skinSettings.chatUsersColorNormal.name() + ";}");
			saved = false;
	}
}

void MainWindow::on_checkBoxBoldUsersNormal_clicked(bool checked)
{
	if (checked)
	{
		skinSettings.chatUsersWeightNormal = 75;
	} else {
		skinSettings.chatUsersWeightNormal = 50;
	}
}

void MainWindow::on_toolButtonColorUsersOwner_clicked()
{
	QColor color = QColorDialog::getColor(skinSettings.chatUsersColorOwner, this);
	if (color.isValid())
	{
			skinSettings.chatUsersColorOwner.setNamedColor(color.name());
			ui->toolButtonColorUsersOwner->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + skinSettings.chatUsersColorOwner.name() + ";}");
			saved = false;
	}
}

void MainWindow::on_checkBoxBoldUsersOwner_clicked(bool checked)
{
	if (checked)
	{
		skinSettings.chatUsersWeightOwner = 75;
	} else {
		skinSettings.chatUsersWeightOwner = 50;
	}
}

void MainWindow::on_toolButtonColorUsersAdministrator_clicked()
{
	QColor color = QColorDialog::getColor(skinSettings.chatUsersColorAdministrator, this);
	if (color.isValid())
	{
			skinSettings.chatUsersColorAdministrator.setNamedColor(color.name());
			ui->toolButtonColorUsersAdministrator->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + skinSettings.chatUsersColorAdministrator.name() + ";}");
			saved = false;
	}
}

void MainWindow::on_checkBoxBoldUsersAdministrator_clicked(bool checked)
{
	if (checked)
	{
		skinSettings.chatUsersWeightAdministrator = 75;
	} else {
		skinSettings.chatUsersWeightAdministrator = 50;
	}
}

void MainWindow::on_toolButtonColorUsersOperator_clicked()
{
	QColor color = QColorDialog::getColor(skinSettings.chatUsersColorOperator, this);
	if (color.isValid())
	{
			skinSettings.chatUsersColorOperator.setNamedColor(color.name());
			ui->toolButtonColorUsersOperator->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + skinSettings.chatUsersColorOperator.name() + ";}");
			saved = false;
	}
}

void MainWindow::on_checkBoxBoldUsersOperator_clicked(bool checked)
{
	if (checked)
	{
		skinSettings.chatUsersWeightOperator = 75;
	} else {
		skinSettings.chatUsersWeightOperator = 50;
	}
}

void MainWindow::on_toolButtonColorUsersHalfOperator_clicked()
{
	QColor color = QColorDialog::getColor(skinSettings.chatUsersColorHalfOperator, this);
	if (color.isValid())
	{
			skinSettings.chatUsersColorHalfOperator.setNamedColor(color.name());
			ui->toolButtonColorUsersHalfOperator->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + skinSettings.chatUsersColorHalfOperator.name() + ";}");
			saved = false;
	}
}

void MainWindow::on_checkBoxBoldUsersHalfOperator_clicked(bool checked)
{
	if (checked)
	{
		skinSettings.chatUsersWeightHalfOperator = 75;
	} else {
		skinSettings.chatUsersWeightHalfOperator = 50;
	}
}

void MainWindow::on_toolButtonColorUsersVoiced_clicked()
{
	QColor color = QColorDialog::getColor(skinSettings.chatUsersColorVoiced, this);
	if (color.isValid())
	{
			skinSettings.chatUsersColorVoiced.setNamedColor(color.name());
			ui->toolButtonColorUsersVoiced->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + skinSettings.chatUsersColorVoiced.name() + ";}");
			saved = false;
	}
}

void MainWindow::on_checkBoxBoldUsersVoiced_clicked(bool checked)
{
	if (checked)
	{
		skinSettings.chatUsersWeightVoiced = 75;
	} else {
		skinSettings.chatUsersWeightVoiced = 50;
	}
}
