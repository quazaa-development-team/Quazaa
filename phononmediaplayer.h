#ifndef PHONONMEDIAPLAYER_H
#define PHONONMEDIAPLAYER_H

#include <QtGui>
#include <phonon>

class phononMediaPlayer : public QWidget
{
Q_OBJECT
public:
	explicit phononMediaPlayer(QWidget *parent = 0, QTableView *fileView = new QTableView());

private slots:
	void play(const QModelIndex &index);

private:
	void delayedInit();

	QTableView *m_fileView;
	QDirModel m_model;

	Phonon::MediaObject *m_media;
signals:

public slots:

};

#endif // PHONONMEDIAPLAYER_H
