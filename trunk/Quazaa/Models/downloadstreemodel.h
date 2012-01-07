/*
** $Id$
**
** Copyright © Quazaa Development Team, 2009-2012.
** This file is part of QUAZAA (quazaa.sourceforge.net)
**
** Quazaa is free software; this file may be used under the terms of the GNU
** General Public License version 3.0 or later as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Quazaa is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** Please review the following information to ensure the GNU General Public
** License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** You should have received a copy of the GNU General Public License version
** 3.0 along with Quazaa; if not, write to the Free Software Foundation,
** Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


#ifndef DOWNLOADSTREEMODEL_H
#define DOWNLOADSTREEMODEL_H

#include <QAbstractItemModel>
#include "types.h"

class CDownload;
class CDownloadSource;
class CDownloadsTreeModel;

class CDownloadsItemBase : public QObject
{
	Q_OBJECT
public:
	CDownloadsItemBase(QObject* parent = 0);
	virtual ~CDownloadsItemBase();

	virtual void appendChild(CDownloadsItemBase* child);
	virtual CDownloadsItemBase* child(int row);
	virtual int childCount() const;
	virtual int columnCount() const;
	virtual QVariant data(int column) const;
	virtual int row() const;
	virtual CDownloadsItemBase* parent();

protected:
	CDownloadsItemBase* parentItem;
	QList<CDownloadsItemBase*> childItems;
	// item data here
};

class CDownloadItem : public CDownloadsItemBase
{
	Q_OBJECT
public:
	CDownloadItem(CDownload* download, CDownloadsItemBase* parent, CDownloadsTreeModel* model, QObject* parentQObject = 0);
	virtual ~CDownloadItem();

	void appendChild(CDownloadsItemBase* child);
	QVariant data(int column) const;
protected:
	CDownload* m_pDownload; // pointer to corresponding CDownload object

	QString m_sName;		// download name
	quint64 m_nSize;		// size
	int		m_nProgress;	// progress in % TODO: change it to something different when nice progress bar will be ready
	qint64	m_nBandwidth;	// total bandwidth, B/s
	int		m_nStatus;		// download status
	int		m_nPriority;	//
	quint64 m_nCompleted;	// bytes completed

	CDownloadsTreeModel* m_pModel;
public slots:
	void onSourceAdded(CDownloadSource* pSource);
};

class CDownloadSourceItem : public CDownloadsItemBase
{
	Q_OBJECT
public:
	CDownloadSourceItem(CDownloadSource* downloadSource, CDownloadsItemBase* parent, QObject* parentQObject = 0);
	virtual ~CDownloadSourceItem();

	void appendChild(CDownloadsItemBase* child);
	QVariant data(int column) const;
protected:
	CDownloadSource* m_pDownloadSource; // pointer to corresponding CDownloadSource object

	QString m_sAddress;		// remote address
	quint64 m_nSize;		// available bytes on remote server/client
	int		m_nProgress;	// unused for now
	qint64	m_nBandwidth;	// bandwidth, B/s
	int		m_nStatus;		// dl source status
	QString m_sClient;		// remote client
	quint64 m_nDownloaded;	// downloaded bytes from this source
	QString m_sCountry;		//
};

class CDownloadsTreeModel : public QAbstractItemModel
{
	Q_OBJECT
public:
	enum Column
	{
		NAME = 0,	// file name / dl source ip
		SIZE,		// file size / available size
		PROGRESS,	// progress / progress from this user
		BANDWIDTH,	// total bandwidth / bandwidth provided by user
		STATUS,		// download status / source status
		PRIORITY,	// download priority / -
		CLIENT,		// - / remote client
		COMPLETED,	// MB completed / MB downloaded
		COUNTRY,	// - / country
		_NO_OF_COLUMNS
	};

public:
	CDownloadsTreeModel(QObject *parent = 0);
	~CDownloadsTreeModel();

	QVariant data(const QModelIndex &index, int role) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &index) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	
private:
	CDownloadsItemBase* rootItem;
signals:
	
public slots:
	void onDownloadAdded(CDownload* pDownload);

	friend class CDownloadItem;
};

#endif // DOWNLOADSTREEMODEL_H
