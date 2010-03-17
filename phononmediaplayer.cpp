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
