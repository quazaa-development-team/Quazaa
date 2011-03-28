//
// widgetmedia.h
//
// Copyright © Quazaa Development Team, 2009-2010.
// This file is part of QUAZAA (quazaa.sourceforge.net)
//
// Quazaa is free software; you can redistribute it
// and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3 of
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

#ifndef WIDGETMEDIA_H
#define WIDGETMEDIA_H

#include <QMainWindow>
#include <QSlider>

namespace Ui
{
	class WidgetMedia;
}

class WidgetMedia : public QMainWindow
{
	Q_OBJECT
public:
	WidgetMedia(QWidget* parent = 0);
	~WidgetMedia();
	QSlider* seekSlider;
	QSlider* volumeSlider;
	void saveWidget();

protected:
	void changeEvent(QEvent* e);

private:
	Ui::WidgetMedia* ui;

private slots:
	void on_toolButtonMediaPlaylistTaskHeader_clicked();
	void on_splitterMedia_customContextMenuRequested(QPoint pos);
	void on_actionMediaShuffle_triggered(bool checked);
	void on_actionMediaRepeat_triggered(bool checked);
	void skinChangeEvent();
};

#endif // WIDGETMEDIA_H
