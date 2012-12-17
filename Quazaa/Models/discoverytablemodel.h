/*
** discoverytablemodel.h
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

#ifndef DISCOVERYTABLEMODEL_H
#define DISCOVERYTABLEMODEL_H

#include <QAbstractTableModel>
#include <QIcon>

#include "Discovery/discoveryservice.h"

class CDiscoveryTableModel : public QAbstractTableModel
{
	Q_OBJECT

private:
	typedef Discovery::TConstServicePtr TConstServicePtr;
	typedef Discovery::TServiceType     TServiceType;
	typedef Discovery::TServiceID       TServiceID;

	QWidget*		m_oContainer;
	int				m_nSortColumn;
	Qt::SortOrder	m_nSortOrder;
	bool			m_bNeedSorting;

public:
	enum Column
	{
		TYPE = 0,
		URL,
		HOSTS,
		TOTAL_HOSTS,
		_NO_OF_COLUMNS
	};

	struct Service
	{
		// Object directly managed by discovery manager.
		TConstServicePtr m_pNode;
		TServiceID       m_nID;

		QString          m_sURL;
		TServiceType     m_nType;
		quint32          m_nLastHosts;
		quint32          m_nTotalHosts;
		QIcon            m_iType;

		Service(TConstServicePtr pService);
		~Service();
		bool update(int row, int col, QModelIndexList& to_update, CDiscoveryTableModel* model);
		QVariant data(int col) const;
		bool lessThan(int col, const CDiscoveryTableModel::Service* const pOther) const;
	};

protected:
	QVector<Service*>   m_lNodes;

public:
	explicit CDiscoveryTableModel(QObject* parent = 0, QWidget* container = 0);
	~CDiscoveryTableModel();

	int rowCount(const QModelIndex& parent = QModelIndex()) const;
	int columnCount(const QModelIndex& parent) const;
	QVariant data(const QModelIndex& index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;

	void sort(int column, Qt::SortOrder order);

	TConstServicePtr nodeFromRow(quint32 row) const;
	TConstServicePtr nodeFromIndex(const QModelIndex& index) const;

	void completeRefresh();

public slots:
	void addService(TConstServicePtr pService);
	void removeService(TServiceID nID);
	void updateAll();

};

#endif // DISCOVERYTABLEMODEL_H
