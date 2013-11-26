//
// widgetmedia.h
//
// Copyright © Quazaa Development Team, 2009-2013.
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

#include "mediaoverlay.h"

#include <QWidget>
#include <QSlider>
#include <QEvent>
#include <QTimer>

namespace Ui
{
	class CWidgetMedia;
}

class CWidgetMedia : public QWidget
{
	Q_OBJECT
public:
	CWidgetMedia(QWidget* parent = 0);
	~CWidgetMedia();
	QSlider* seekSlider;
	QSlider* volumeSlider;
	void saveWidget();

protected:
	void changeEvent(QEvent* e);
	void mouseMoveEvent(QMouseEvent *e);

private:
	Ui::CWidgetMedia* ui;

private slots:
	void on_toolButtonMediaPlaylistTaskHeader_clicked();
	void on_splitterMedia_customContextMenuRequested(QPoint pos);
	void on_actionMediaShuffle_triggered(bool checked);
	void on_actionMediaRepeat_triggered(bool checked);
	void setSkin();

private:
	QTimer* m_tMediaControls;
	QTimer* m_tVolumeControl;
};

#endif // WIDGETMEDIA_H
