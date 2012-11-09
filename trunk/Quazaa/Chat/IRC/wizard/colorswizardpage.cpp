/*
* Copyright (C) 2008-2011 J-P Nurmi jpnurmi@gmail.com
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*/

#include "colorswizardpage.h"

ColorsWizardPage::ColorsWizardPage(QWidget* parent) : QWizardPage(parent)
{
	ui.setupUi(this);
	setPixmap(QWizard::LogoPixmap, QPixmap(":/Resource/oxygen/64x64/actions/color_line.png"));
	m_oColorsDelegate = new ColorItemDelegate(ui.treeWidget);
	ui.treeWidget->setItemDelegate(m_oColorsDelegate);

	connect(m_oColorsDelegate, SIGNAL(colorChanged()), this, SIGNAL(settingsChanged()));
}

QHash<int, QString> ColorsWizardPage::colors() const
{
	QHash<int, QString> colors;
	for (int i = IrcColorType::Background; i <= IrcColorType::Highlight; ++i)
		colors[i] = ui.treeWidget->topLevelItem(i)->data(Color, Qt::DisplayRole).toString();
	return colors;
}

void ColorsWizardPage::setColors(const QHash<int, QString>& colors)
{
	QHashIterator<int, QString> it(colors);
	while (it.hasNext())
	{
		it.next();
		ui.treeWidget->topLevelItem(it.key())->setData(Color, Qt::DisplayRole, it.value());
		ui.treeWidget->topLevelItem(it.key())->setData(Color, Qt::DecorationRole, QColor(it.value()));
	}
}
