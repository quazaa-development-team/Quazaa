/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>
#include <QtWebKit>
#include <QDockWidget>
#include <QTextEdit>
#include "mainwindow.h"

MainWindow::MainWindow()
{
    progress = 0;
    firstRequest = true;

    QFile file;
    file.setFileName(":/jquery.min.js");
    file.open(QIODevice::ReadOnly);
    jQuery = file.readAll();
    file.close();

    QNetworkProxyFactory::setUseSystemConfiguration(true);

    view = new QWebView(this);
    view->load(QUrl("http://www.google.com/ncr"));

    // using loadStarted signal "should" be better
    connect(view, SIGNAL(loadFinished(bool)), SLOT(adjustLocation()));
    connect(view, SIGNAL(titleChanged(QString)), SLOT(adjustTitle()));
    connect(view, SIGNAL(loadProgress(int)), SLOT(setProgress(int)));
    connect(view, SIGNAL(loadFinished(bool)), SLOT(finishLoading(bool)));

    connect(view->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(populateJavaScriptWindowObject()));

    locationEdit = new QLineEdit(this);
    locationEdit->setSizePolicy(QSizePolicy::Expanding, locationEdit->sizePolicy().verticalPolicy());
    connect(locationEdit, SIGNAL(returnPressed()), SLOT(changeLocation()));

    QToolBar *toolBar = addToolBar(tr("Navigation"));
    toolBar->addAction(view->pageAction(QWebPage::Back));
    toolBar->addAction(view->pageAction(QWebPage::Forward));
    toolBar->addAction(view->pageAction(QWebPage::Reload));
    toolBar->addAction(view->pageAction(QWebPage::Stop));
    toolBar->addWidget(locationEdit);

    QDockWidget *dockWidget = new QDockWidget(tr("Requests"), this);
    dockWidget->setFeatures(QDockWidget::NoDockWidgetFeatures);
    dockWidget->setAllowedAreas(Qt::BottomDockWidgetArea);

    QTextDocument *doc = new QTextDocument(this);
    doc->setDefaultStyleSheet("div { margin-top: 1px; margin-bottom: 1px; }");
    logPane = new QTextEdit(dockWidget);
    logPane->setReadOnly(true);
    logPane->setDocument(doc);

    dockWidget->setWidget(logPane);
    addDockWidget(Qt::BottomDockWidgetArea, dockWidget);

    setCentralWidget(view);
    setUnifiedTitleAndToolBarOnMac(true);
}

void MainWindow::adjustLocation()
{
    locationEdit->setText(view->url().toString());
    logPane->moveCursor(QTextCursor::End);
    if (!firstRequest) {
        logPane->insertHtml("<hr /><div></div>"); // otherwise it'll go nuts! lol
    }
    logPane->moveCursor(QTextCursor::End);
    firstRequest = false;

    if (postString.isNull())
    {
        logPane->insertHtml("<div><b>GET:</b> " + view->url().toString() + "</div>");
    } else {
        logPane->insertHtml("<div><b>POST:</b> " + view->url().toString() + " <b>Data:</b> " + postString + "</div>");
    }

    logPane->ensureCursorVisible(); // scrolling

    postString = QString::null;
}

void MainWindow::changeLocation()
{
    QUrl url = QUrl(locationEdit->text());
    view->load(url);
    view->setFocus();
}

void MainWindow::adjustTitle()
{
    if (progress <= 0 || progress >= 100)
        setWindowTitle("AutoRouterConfig : " + view->title());
    else
        setWindowTitle("AutoRouterConfig : " + QString("%1 (%2%)").arg(view->title()).arg(progress));
}

void MainWindow::setProgress(int p)
{
    progress = p;
    adjustTitle();
}

void MainWindow::finishLoading(bool)
{
    progress = 100;
    adjustTitle();
    view->page()->mainFrame()->evaluateJavaScript(jQuery);
    QString code = "$('form').each( function() { $(this).submit(function() { formExtractor.formValues($(this).serialize()); return true; }) } )";
    view->page()->mainFrame()->evaluateJavaScript(code);
}

// This function will connect formExtractor to this class

void MainWindow::populateJavaScriptWindowObject()
{
    view->page()->mainFrame()->addToJavaScriptWindowObject("formExtractor", this);
}

// The function that will be called from the injected javascript

void MainWindow::formValues(QString str) {
    postString = str;
}

