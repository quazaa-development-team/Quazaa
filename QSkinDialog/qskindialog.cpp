/********************************************************************************************************
 * PROGRAM		: QSkinDialog
 * DATE - TIME	: 27 December 2009 - 14h38
 * AUTHOR		: (SmokeX)
 * FILENAME		: qskindialog.cpp
 * LICENSE		: QSkinDialog is free software; you can redistribute it
 *				  and/or modify it under the terms of the GNU General Public License
 *				  as published by the Free Software Foundation; either version 3 of
 *				  the License, or (at your option) any later version.
 * COMMENTARY   : The core of QSkinDialog, a library allowing skinning in Qt through
 *				  style sheets. It is written for reuse in multiple programs.
 ********************************************************************************************************/
#include "qskindialog.h"
#include "ui_qskindialog.h"
#include "qskinconfiguredialog.h"
#include "mainwindow.h"
#include "qskinsettings.h"
#include "quazaasettings.h"

QSkinDialog::QSkinDialog(bool sizable, bool closable, bool mainDialog, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::QSkinDialog)
{
	ui->setupUi(this);
	maximized = false;
	minimized = false;
	movable = false;
	dialogSizable = sizable;
	dialogClosable = closable;
	isMainDialog = mainDialog;
	sizableTopLeft = false;
	sizableLeft = false;
	sizableBottomLeft = false;
	sizableBottom = false;
	sizableTopRight = false;
	sizableRight = false;
	sizableBottomRight = false;

	systemMenu = new QMenu(this);
	systemRestoreAction = new QAction(tr("Restore"), this);
	systemRestoreAction->setIcon(style()->standardIcon(QStyle::SP_TitleBarNormalButton));
	systemRestoreAction->setEnabled(false);
	systemMenu->addAction(systemRestoreAction);
	systemMinimizeAction = new QAction(tr("Minimize"), this);
	systemMinimizeAction->setIcon(style()->standardIcon(QStyle::SP_TitleBarMinButton));
	systemMenu->addAction(systemMinimizeAction);
	systemMinimizeAction->setEnabled(dialogSizable);
	systemMaximizeAction = new QAction(tr("Maximize"), this);
	systemMaximizeAction->setIcon(style()->standardIcon(QStyle::SP_TitleBarMaxButton));
	systemMenu->addAction(systemMaximizeAction);
	systemMaximizeAction->setEnabled(dialogSizable);
	systemMenu->addSeparator();
	systemCloseAction = new QAction(tr("Close"), this);
	systemCloseAction->setIcon(style()->standardIcon(QStyle::SP_TitleBarCloseButton));
	systemCloseAction->setShortcut(QKeySequence::Close);
	systemCloseAction->setEnabled(dialogClosable);
	systemMenu->addAction(systemCloseAction);

	// To enable alpha blending and transparency in the frame
	setAttribute(Qt::WA_TranslucentBackground);
	// We are using our own frame of course instead of the system frame
	if(!parent == 0)
	{
		this->setWindowFlags(Qt::FramelessWindowHint|Qt::Window);
	} else {
		this->setWindowFlags(Qt::FramelessWindowHint);
	}
	this->setMaximumSize(QApplication::desktop()->availableGeometry(this).width(), QApplication::desktop()->availableGeometry(this).height());

	ui->minimizeButton->setEnabled(dialogSizable);
	ui->maximizeButton->setEnabled(dialogSizable);
	ui->closeButton->setEnabled(dialogClosable);
	if (!dialogSizable)
	{
		ui->windowFrameTopLeft->setCursor(QCursor(Qt::ArrowCursor));
		ui->windowFrameLeft->setCursor(QCursor(Qt::ArrowCursor));
		ui->windowFrameBottomLeft->setCursor(QCursor(Qt::ArrowCursor));
		ui->windowFrameTop->setCursor(QCursor(Qt::ArrowCursor));
		ui->windowFrameBottom->setCursor(QCursor(Qt::ArrowCursor));
		ui->windowFrameTopRight->setCursor(QCursor(Qt::ArrowCursor));
		ui->windowFrameRight->setCursor(QCursor(Qt::ArrowCursor));
		ui->windowFrameBottomRight->setCursor(QCursor(Qt::ArrowCursor));
	}

	connect(systemRestoreAction, SIGNAL(triggered()), this, SLOT(onMaximize()));
	connect(systemMaximizeAction, SIGNAL(triggered()), this, SLOT(onMaximize()));
	connect(systemMinimizeAction, SIGNAL(triggered()), this, SLOT(onMinimize()));
	connect(systemCloseAction, SIGNAL(triggered()), this, SLOT(onClose()));
	connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(onClose()));
	connect(ui->minimizeButton, SIGNAL(clicked()), this, SLOT(onMinimize()));
	connect(ui->maximizeButton, SIGNAL(clicked()), this, SLOT(onMaximize()));
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));

	if(quazaaSettings.SkinFile.isEmpty())
	{
		quazaaSettings.loadSkinSettings();
	}

	skinSettings.loadSkin(quazaaSettings.SkinFile);
	quazaaSettings.saveSkinSettings();

	// Load the previously set skin
	ui->windowFrameTopLeft->setStyleSheet(skinSettings.windowFrameTopLeftStyleSheet);
	ui->windowFrameLeft->setStyleSheet(skinSettings.windowFrameLeftStyleSheet);
	ui->windowFrameBottomLeft->setStyleSheet(skinSettings.windowFrameBottomLeftStyleSheet);
	ui->windowFrameTop->setStyleSheet(skinSettings.windowFrameTopStyleSheet);
	ui->windowFrameBottom->setStyleSheet(skinSettings.windowFrameBottomStyleSheet);
	ui->windowFrameTopRight->setStyleSheet(skinSettings.windowFrameTopRightStyleSheet);
	ui->windowFrameRight->setStyleSheet(skinSettings.windowFrameRightStyleSheet);
	ui->windowFrameBottomRight->setStyleSheet(skinSettings.windowFrameBottomRightStyleSheet);
	ui->titlebarFrame->setStyleSheet(skinSettings.titlebarFrameStyleSheet);
	ui->titlebarButtonsFrame->setStyleSheet(skinSettings.titlebarButtonsFrameStyleSheet);
	ui->minimizeButton->setStyleSheet(skinSettings.minimizeButtonStyleSheet);
	ui->maximizeButton->setStyleSheet(skinSettings.maximizeButtonStyleSheet);
	ui->closeButton->setStyleSheet(skinSettings.closeButtonStyleSheet);
	ui->windowText->setStyleSheet(skinSettings.windowTextStyleSheet);
	ui->windowIcon->setVisible(skinSettings.windowIconVisible);
	ui->windowIcon->setIconSize(skinSettings.windowIconSize);

	if (isMainDialog)
		quazaaSettings.loadSkinWindowSettings(this);
}

QSkinDialog::~QSkinDialog()
{
	delete ui;
}

void QSkinDialog::changeEvent(QEvent *e)
{
	QDialog::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	case QEvent::ActivationChange:
		if (minimized)
		{
			setMinimized(false);
		}
		break;
	default:
		break;
	}
}

void QSkinDialog::onClose()
{
	if (isMainDialog)
	{
		quazaaSettings.saveSkinWindowSettings(this);
		emit mainClose();
	} else {
		close();
	}
}

void QSkinDialog::onMaximize()
{
	setMaximized(!maximized);
}

// Custom maximize event because showMaximized resizes the window to the entire desktop (including
// over panels and the taskbar) when FramelessWindowHint is used.
void QSkinDialog::setMaximized(bool maximize)
{
	ui->maximizeButton->setChecked(maximize);
	if (maximize)
	{
		normalGeometry = this->geometry();
		setGeometry(0,0,QApplication::desktop()->availableGeometry(this).width(),QApplication::desktop()->availableGeometry(this).height());
		maximized = true;
		ui->maximizeButton->setToolTip("Restore Down");
		systemRestoreAction->setEnabled(true);
		systemMaximizeAction->setEnabled(false);
		ui->windowFrameTopLeft->setVisible(false);
		ui->windowFrameLeft->setVisible(false);
		ui->windowFrameBottomLeft->setVisible(false);
		ui->windowFrameBottom->setVisible(false);
		ui->windowFrameTopRight->setVisible(false);
		ui->windowFrameRight->setVisible(false);
		ui->windowFrameBottomRight->setVisible(false);
	} else {
		this->setGeometry(normalGeometry);
		maximized = false;
		ui->maximizeButton->setToolTip("Maximize");
		systemRestoreAction->setEnabled(false);
		systemMaximizeAction->setEnabled(true);
		ui->windowFrameTopLeft->setVisible(true);
		ui->windowFrameLeft->setVisible(true);
		ui->windowFrameBottomLeft->setVisible(true);
		ui->windowFrameBottom->setVisible(true);
		ui->windowFrameTopRight->setVisible(true);
		ui->windowFrameRight->setVisible(true);
		ui->windowFrameBottomRight->setVisible(true);
	}
}

void QSkinDialog::onMinimize()
{
	if (isMainDialog && quazaaSettings.BasicMinimizeToTray)
		hide();
	else
		setMinimized(!minimized);
}

void QSkinDialog::setMinimized(bool minimize)
{
	if (minimize)
	{
		if (!maximized)
			normalGeometry = this->geometry();

		setGeometry(QApplication::desktop()->availableGeometry(this).width(),QApplication::desktop()->availableGeometry(this).height(),0,0);
		minimized = true;
	} else {
		if (maximized)
			setGeometry(0,0,QApplication::desktop()->availableGeometry(this).width(),QApplication::desktop()->availableGeometry(this).height());
		else
			this->setGeometry(normalGeometry);
		minimized = false;
	}
}

// These are checks to make sure erroneous move and resize events aren't triggered when the mouse is moved
// over the window frame with the left mouse button held down. Mime drags from another app for example.
void QSkinDialog::mousePressEvent(QMouseEvent *e)
{
	switch (e->button())
	{
		case Qt::LeftButton:
		if((ui->windowFrameTop->underMouse()  || ui->windowText->underMouse()) && !ui->windowIcon->underMouse() && !ui->closeButton->underMouse()
			&& !ui->minimizeButton->underMouse() && !ui->maximizeButton->underMouse() && !maximized)
		{
			QApplication::setOverrideCursor(QCursor(Qt::SizeAllCursor));
			dragPosition = e->globalPos() - frameGeometry().topLeft();
			movable = true;
			e->accept();
		} else if (ui->windowFrameTopLeft->underMouse() && dialogSizable) {
			dragPosition = e->globalPos() - frameGeometry().topLeft();
			sizableTopLeft = true;
			e->accept();
		} else if (ui->windowFrameLeft->underMouse() && dialogSizable) {
			iDragPosition = e->globalPos().x() - frameGeometry().left();
			this->sizableLeft = true;
			e->accept();
		} else if (ui->windowFrameBottomLeft->underMouse() && dialogSizable) {
			dragPosition = e->globalPos() - frameGeometry().bottomLeft();
			sizableBottomLeft = true;
			e->accept();
		} else if (ui->windowFrameBottom->underMouse() && dialogSizable) {
			iDragPosition = e->globalPos().y() - frameGeometry().bottom();
			sizableBottom = true;
			e->accept();
		} else if (ui->windowFrameTopRight->underMouse() && dialogSizable) {
			dragPosition = e->globalPos() - frameGeometry().topRight();
			sizableTopRight = true;
			e->accept();
		} else if (ui->windowFrameRight->underMouse() && dialogSizable) {
			iDragPosition = e->globalPos().x() - frameGeometry().right();
			sizableRight = true;
			e->accept();
		} else if (ui->windowFrameBottomRight->underMouse() && dialogSizable) {
			dragPosition = e->globalPos() - frameGeometry().bottomRight();
			sizableBottomRight = true;
			e->accept();
		} else {
			movable = false;
			sizableTopLeft = false;
			sizableLeft = false;
			sizableBottomLeft = false;
			sizableBottom = false;
			sizableTopRight = false;
			sizableRight = false;
			sizableBottomRight = false;
			e->accept();
		}
		break;

		default:
		e->ignore();
		break;
	}
}

// All your drag move and resize events on the window frame
void QSkinDialog::mouseMoveEvent(QMouseEvent *e)
{
	if ((e->buttons() & Qt::LeftButton) && (ui->windowFrameTop->underMouse()  || ui->windowText->underMouse()) && !ui->windowIcon->underMouse() && !ui->closeButton->underMouse()
		&& !ui->minimizeButton->underMouse() && !ui->maximizeButton->underMouse() && movable && !maximized)
	{
		move(e->globalPos() - dragPosition);
		normalGeometry = this->geometry();
		e->accept();
	} else if (ui->windowFrameTopLeft->underMouse() && sizableTopLeft && dialogSizable) {
		QRect newSize = geometry();
		newSize.setTopLeft(e->globalPos() - dragPosition);
		setGeometry(newSize);
		normalGeometry = newSize;
		e->accept();
	} else if (ui->windowFrameLeft->underMouse() && sizableLeft && dialogSizable) {
		QRect newSize = geometry();
		newSize.setLeft(e->globalPos().x() - iDragPosition);
		setGeometry(newSize);
		normalGeometry = newSize;
		e->accept();
	} else if (ui->windowFrameBottomLeft->underMouse() && sizableBottomLeft && dialogSizable) {
		QRect newSize = geometry();
		newSize.setBottomLeft(e->globalPos() - dragPosition);
		setGeometry(newSize);
		normalGeometry = newSize;
		e->accept();
	} else if (ui->windowFrameBottom->underMouse() && sizableBottom && dialogSizable) {
		QRect newSize = geometry();
		newSize.setBottom(e->globalPos().y() - iDragPosition);
		setGeometry(newSize);
		normalGeometry = newSize;
		e->accept();
	} else if (ui->windowFrameTopRight->underMouse() && sizableTopRight && dialogSizable) {
		QRect newSize = geometry();
		newSize.setTopRight(e->globalPos() - dragPosition);
		setGeometry(newSize);
		normalGeometry = newSize;
		e->accept();
	} else if (ui->windowFrameRight->underMouse() && sizableRight && dialogSizable) {
		QRect newSize = geometry();
		newSize.setRight(e->globalPos().x() - iDragPosition);
		setGeometry(newSize);
		normalGeometry = newSize;
		e->accept();
	} else if (ui->windowFrameBottomRight->underMouse() && sizableBottomRight && dialogSizable) {
		QRect newSize = geometry();
		newSize.setBottomRight(e->globalPos() - dragPosition);
		setGeometry(newSize);
		normalGeometry = newSize;
		e->accept();
	}
}

// Remove the "move" mouse pointer for the drag move event on the titlebar
void QSkinDialog::mouseReleaseEvent(QMouseEvent *e)
{
	if (e->button() == Qt::LeftButton && (ui->windowFrameTop->underMouse() || ui->windowText->underMouse()))
	{
		while (QApplication::overrideCursor() != 0)
		QApplication::restoreOverrideCursor();
	}
}

void QSkinDialog::mouseDoubleClickEvent(QMouseEvent *e)
{
	if ((e->button() == Qt::LeftButton) && (ui->windowFrameTop->underMouse()  || ui->windowText->underMouse()) && !ui->windowIcon->underMouse() && !ui->closeButton->underMouse()
		&& !ui->minimizeButton->underMouse() && !ui->maximizeButton->underMouse() && dialogSizable)
	{
		onMaximize();
	}
}

void QSkinDialog::loadSkin(QString file)
{

}

void QSkinDialog::on_windowText_customContextMenuRequested(QPoint pos)
{
	systemMenu->exec(QCursor::pos());
}

void QSkinDialog::on_windowFrameTop_customContextMenuRequested(QPoint pos)
{
	systemMenu->exec(QCursor::pos());
}

void QSkinDialog::addChildWidget(QWidget *parent)
{
	// Unset the frameless window hint for our child widget or it won't display at runtime
	// AutoFillbackground has to be set also or the child gets erased in the transparency mask
	// In this example it is set in the ui files
	parent->setWindowFlags(ui->widgetContents->windowFlags() & ~Qt::FramelessWindowHint);
	ui->layoutContents->addWidget(parent);
	setWindowTitle(parent->windowTitle());
	setWindowIcon(parent->windowIcon());
}

void QSkinDialog::setWindowTitle(const QString &title)
{
	ui->windowText->setText(title);
	return QDialog::setWindowTitle(title);
}

void QSkinDialog::setWindowIcon(const QIcon &icon)
{
	QIcon windowIcon = icon;
	if (windowIcon.isNull())
	{
		windowIcon = QIcon(":/Resource/qtlogo-64.png");
	}

	ui->windowIcon->setIcon(windowIcon);
	return QDialog::setWindowIcon(windowIcon);
}

void QSkinDialog::on_windowIcon_customContextMenuRequested(QPoint pos)
{
	// This is where your custom right click menu would go for the window icon
	systemMenu->exec(ui->windowIcon->mapToGlobal(ui->windowIcon->geometry().bottomLeft()));
}

void QSkinDialog::on_windowIcon_clicked()
{
	systemMenu->exec(ui->windowIcon->mapToGlobal(ui->windowIcon->geometry().bottomLeft()));
}


// Catches the skinChanged() signal from qskinsettings and sould change skins
// in all skinned dialogs
void QSkinDialog::skinChangeEvent()
{
	ui->windowFrameTopLeft->setStyleSheet(skinSettings.windowFrameTopLeftStyleSheet);
	ui->windowFrameLeft->setStyleSheet(skinSettings.windowFrameLeftStyleSheet);
	ui->windowFrameBottomLeft->setStyleSheet(skinSettings.windowFrameBottomLeftStyleSheet);
	ui->windowFrameTop->setStyleSheet(skinSettings.windowFrameTopStyleSheet);
	ui->windowFrameBottom->setStyleSheet(skinSettings.windowFrameBottomStyleSheet);
	ui->windowFrameTopRight->setStyleSheet(skinSettings.windowFrameTopRightStyleSheet);
	ui->windowFrameRight->setStyleSheet(skinSettings.windowFrameRightStyleSheet);
	ui->windowFrameBottomRight->setStyleSheet(skinSettings.windowFrameBottomRightStyleSheet);
	ui->titlebarFrame->setStyleSheet(skinSettings.titlebarFrameStyleSheet);
	ui->titlebarButtonsFrame->setStyleSheet(skinSettings.titlebarButtonsFrameStyleSheet);
	ui->minimizeButton->setStyleSheet(skinSettings.minimizeButtonStyleSheet);
	ui->maximizeButton->setStyleSheet(skinSettings.maximizeButtonStyleSheet);
	ui->closeButton->setStyleSheet(skinSettings.closeButtonStyleSheet);
	ui->windowText->setStyleSheet(skinSettings.windowTextStyleSheet);
	ui->windowIcon->setVisible(skinSettings.windowIconVisible);
	ui->windowIcon->setIconSize(skinSettings.windowIconSize);
}
