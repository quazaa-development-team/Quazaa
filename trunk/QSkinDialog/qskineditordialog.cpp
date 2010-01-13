/********************************************************************************************************
 * PROGRAM		: QSkinDialog
 * DATE - TIME	: 27 December 2009 - 14h38
 * AUTHOR		: (SmokeX)
 * FILENAME		: qskineditordialog.cpp
 * LICENSE		: QSkinDialog is free software; you can redistribute it
 *				  and/or modify it under the terms of the GNU General Public License
 *				  as published by the Free Software Foundation; either version 3 of
 *				  the License, or (at your option) any later version.
 * COMMENTARY   : A simple editor without the ability to actually edit or create a skin.
 *				  It just creates a skin file based on the current skin which is created
 *				  at design time.
 ********************************************************************************************************/
#include "qskineditordialog.h"
#include "ui_qskineditordialog.h"
#include "qskinsettings.h"
#include <QFileDialog>

QSkinEditorDialog::QSkinEditorDialog(bool sizable, bool closable, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::QSkinEditorDialog)
{
	ui->setupUi(this);
	connect(ui->pushButtonSaveSkin, SIGNAL(clicked()), this, SLOT(saveSkin()));
	maximized = false;
	movable = false;
	dialogSizable = sizable;
	dialogClosable = closable;
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

	// Unset the frameless window hint for our child widget or it won't display at runtime
	// AutoFillbackground has to be set also or the child gets erased in the transparency mask
	// In this example it is set in the ui file
	ui->widgetContents->setWindowFlags(ui->widgetContents->windowFlags() & ~Qt::FramelessWindowHint);

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
}

QSkinEditorDialog::~QSkinEditorDialog()
{
	delete ui;
}

void QSkinEditorDialog::changeEvent(QEvent *e)
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

void QSkinEditorDialog::onClose()
{
	close();
}

void QSkinEditorDialog::onMaximize()
{
	setMaximized(!maximized);
}

void QSkinEditorDialog::setMaximized(bool maximize)
{
	ui->maximizeButton->setChecked(maximize);
	if (maximize)
	{
		normalGeometry = this->geometry();
		setGeometry(0,0,QApplication::desktop()->availableGeometry(this).width(),QApplication::desktop()->availableGeometry(this).height());
		maximized = true;
		ui->maximizeButton->setToolTip("Restore");
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

void QSkinEditorDialog::onMinimize()
{
	setMinimized(!minimized);
}

void QSkinEditorDialog::setMinimized(bool minimize)
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

void QSkinEditorDialog::mousePressEvent(QMouseEvent *e)
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

void QSkinEditorDialog::mouseMoveEvent(QMouseEvent *e)
{
	if ((e->buttons() & Qt::LeftButton) && (ui->windowFrameTop->underMouse()  || ui->windowText->underMouse()) && !ui->windowIcon->underMouse() && !ui->closeButton->underMouse()
		&& !ui->minimizeButton->underMouse() && !ui->maximizeButton->underMouse() && movable && !maximized)
	{
		move(e->globalPos() - dragPosition);
		e->accept();
	} else if (ui->windowFrameTopLeft->underMouse() && sizableTopLeft && dialogSizable) {
		QRect newSize = geometry();
		newSize.setTopLeft(e->globalPos() - dragPosition);
		setGeometry(newSize);
		e->accept();
	} else if (ui->windowFrameLeft->underMouse() && sizableLeft && dialogSizable) {
		QRect newSize = geometry();
		newSize.setLeft(e->globalPos().x() - iDragPosition);
		setGeometry(newSize);
		e->accept();
	} else if (ui->windowFrameBottomLeft->underMouse() && sizableBottomLeft && dialogSizable) {
		QRect newSize = geometry();
		newSize.setBottomLeft(e->globalPos() - dragPosition);
		setGeometry(newSize);
		e->accept();
	} else if (ui->windowFrameBottom->underMouse() && sizableBottom && dialogSizable) {
		QRect newSize = geometry();
		newSize.setBottom(e->globalPos().y() - iDragPosition);
		setGeometry(newSize);
		e->accept();
	} else if (ui->windowFrameTopRight->underMouse() && sizableTopRight && dialogSizable) {
		QRect newSize = geometry();
		newSize.setTopRight(e->globalPos() - dragPosition);
		setGeometry(newSize);
		e->accept();
	} else if (ui->windowFrameRight->underMouse() && sizableRight && dialogSizable) {
		QRect newSize = geometry();
		newSize.setRight(e->globalPos().x() - iDragPosition);
		setGeometry(newSize);
		e->accept();
	} else if (ui->windowFrameBottomRight->underMouse() && sizableBottomRight && dialogSizable) {
		QRect newSize = geometry();
		newSize.setBottomRight(e->globalPos() - dragPosition);
		setGeometry(newSize);
		e->accept();
	}
}

void QSkinEditorDialog::mouseReleaseEvent(QMouseEvent *e)
{
	if (e->button() == Qt::LeftButton && (ui->windowFrameTop->underMouse() || ui->windowText->underMouse()))
	{
		while (QApplication::overrideCursor() != 0)
		QApplication::restoreOverrideCursor();
	}
}

void QSkinEditorDialog::mouseDoubleClickEvent(QMouseEvent *e)
{
	if ((e->button() == Qt::LeftButton) && (ui->windowFrameTop->underMouse()  || ui->windowText->underMouse()) && !ui->windowIcon->underMouse() && !ui->closeButton->underMouse()
		&& !ui->minimizeButton->underMouse() && !ui->maximizeButton->underMouse() && dialogSizable)
	{
		onMaximize();
	}
}

void QSkinEditorDialog::loadSkin(QString file)
{

}

void QSkinEditorDialog::on_windowText_customContextMenuRequested(QPoint pos)
{
	systemMenu->exec(QCursor::pos());
}

void QSkinEditorDialog::on_windowFrameTop_customContextMenuRequested(QPoint pos)
{
	systemMenu->exec(QCursor::pos());
}

void QSkinEditorDialog::setWindowTitle(const QString &title)
{
	ui->windowText->setText(title);
	return QDialog::setWindowTitle(title);
}

void QSkinEditorDialog::setWindowIcon(const QIcon &icon)
{
	QIcon windowIcon = icon;
	if (windowIcon.isNull())
	{
		windowIcon = QIcon(":/Resource/qtlogo-64.png");
	}

	ui->windowIcon->setIcon(windowIcon);
	return QDialog::setWindowIcon(windowIcon);
}

void QSkinEditorDialog::on_windowIcon_customContextMenuRequested(QPoint pos)
{
	// This is where your custom right click menu would go for the window icon
	systemMenu->exec(ui->windowIcon->mapToGlobal(ui->windowIcon->geometry().bottomLeft()));
}

void QSkinEditorDialog::on_windowIcon_clicked()
{
	systemMenu->exec(ui->windowIcon->mapToGlobal(ui->windowIcon->geometry().bottomLeft()));
}

void QSkinEditorDialog::skinChangeEvent()
{

}

void QSkinEditorDialog::saveSkin()
{
	QSkinSettings *tempSkinSettings = new QSkinSettings();
	tempSkinSettings->closeButtonStyleSheet = ui->closeButton->styleSheet().replace(QString(":/Resource"), QString("Skin"));
	tempSkinSettings->maximizeButtonStyleSheet = ui->maximizeButton->styleSheet().replace(QString(":/Resource"), QString("Skin"));
	tempSkinSettings->minimizeButtonStyleSheet = ui->minimizeButton->styleSheet().replace(QString(":/Resource"), QString("Skin"));
	tempSkinSettings->skinAuthor = ui->lineEditSkinAuthor->text();
	tempSkinSettings->skinName = ui->lineEditSkinName->text();
	tempSkinSettings->skinVersion = ui->lineEditSkinVersion->text();
	tempSkinSettings->skinDescription = ui->plainTextEditSkinDescription->toPlainText();
	tempSkinSettings->titlebarButtonsFrameStyleSheet = ui->titlebarButtonsFrame->styleSheet().replace(QString(":/Resource"), QString("Skin"));
	tempSkinSettings->titlebarFrameStyleSheet = ui->titlebarFrame->styleSheet().replace(QString(":/Resource"), QString("Skin"));
	tempSkinSettings->windowFrameBottomLeftStyleSheet = ui->windowFrameBottomLeft->styleSheet().replace(QString(":/Resource"), QString("Skin"));
	tempSkinSettings->windowFrameBottomRightStyleSheet = ui->windowFrameBottomRight->styleSheet().replace(QString(":/Resource"), QString("Skin"));
	tempSkinSettings->windowFrameBottomStyleSheet = ui->windowFrameBottom->styleSheet().replace(QString(":/Resource"), QString("Skin"));
	tempSkinSettings->windowFrameLeftStyleSheet = ui->windowFrameLeft->styleSheet().replace(QString(":/Resource"), QString("Skin"));
	tempSkinSettings->windowFrameRightStyleSheet = ui->windowFrameRight->styleSheet().replace(QString(":/Resource"), QString("Skin"));
	tempSkinSettings->windowFrameTopLeftStyleSheet = ui->windowFrameTopLeft->styleSheet().replace(QString(":/Resource"), QString("Skin"));
	tempSkinSettings->windowFrameTopRightStyleSheet = ui->windowFrameTopRight->styleSheet().replace(QString(":/Resource"), QString("Skin"));
	tempSkinSettings->windowFrameTopStyleSheet = ui->windowFrameTop->styleSheet().replace(QString(":/Resource"), QString("Skin"));
	tempSkinSettings->windowTextStyleSheet = ui->windowText->styleSheet().replace(QString(":/Resource"), QString("Skin"));
	tempSkinSettings->windowIconSize = ui->windowIcon->iconSize();
	tempSkinSettings->windowIconVisible = ui->windowIcon->isVisible();

	QString fileName = QFileDialog::getSaveFileName(0, tr("Save Skin"),
													QApplication::applicationDirPath(),
													tr("Quazaa Skin (*.qsk)"));

	if (!fileName.isEmpty())
	{
		tempSkinSettings->saveSkin(fileName);
	}
}
