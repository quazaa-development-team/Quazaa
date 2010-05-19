//
// phonomediaplayer.cpp
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

#include "phononmediaplayer.h"

#include <phonon/mediaobject.h>
#include <phonon/path.h>
#include <phonon/audiooutput.h>

#include <QtGui/QApplication>
#include <QtGui/QMainWindow>
#include <QtGui/QDirModel>
#include <QtGui/QColumnView>

phononMediaPlayer::phononMediaPlayer(QWidget *parent, QTableView *fileView) :
	QWidget(parent) ,
	m_media(0)
{
	m_fileView = fileView;
	m_fileView->setModel(&m_model);

	connect(m_fileView, SIGNAL(updatePreviewWidget(const QModelIndex &)), this, SLOT(play(const QModelIndex &)));
}

void phononMediaPlayer::play(const QModelIndex &index)
{
	delayedInit();
	m_media->setCurrentSource(m_model.filePath(index));
	m_media->play();
}

void phononMediaPlayer::delayedInit()
{
	 if (!m_media) {
		m_media = new Phonon::MediaObject(this);
		Phonon::AudioOutput *audioOutput = new Phonon::AudioOutput(Phonon::MusicCategory, this);
		createPath(m_media, audioOutput);
	}
}
