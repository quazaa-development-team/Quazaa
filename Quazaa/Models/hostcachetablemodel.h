/*
** hostcachetablemodel.h
**
** Copyright © Quazaa Development Team, 2013-2013.
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

#ifndef G2CACHETABLEMODEL_H
#define G2CACHETABLEMODEL_H

#include <QAbstractTableModel>

#include "g2hostcache.h"

class HostCacheTableModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	struct HostData
	{
		SharedHostPtr   m_pHost;

		const CEndPoint m_oAddress;
		const QString   m_sAddress;
		const QString   m_sCountryCode;
		const QString   m_sCountry;
		const QIcon     m_iCountry;
		const quint32   m_nID;
		quint32         m_tLastConnect;
		QString         m_sLastConnect;
		quint8          m_nFailures;
		QString         m_sFailures;

		//QString         m_sUserAgent;
		//QIcon           m_iNetwork;

		HostData(SharedHostPtr pHost);
		bool update(int row, int col, QModelIndexList& to_update, HostCacheTableModel* model);
		QVariant data(int col) const;
		bool lessThan(int col, HostData* pOther) const;
	};

private:
	QWidget*        m_oContainer;
	Qt::SortOrder   m_nSortOrder;
	int             m_nSortColumn;
	bool            m_bNeedSorting;

	quint32         m_nHostInfo;

protected:
	std::vector<HostData*> m_vHosts;

public:
	enum Column
	{
		ADDRESS        = 0,
		LASTCONNECT    = 1,
		FAILURES       = 2,
		COUNTRY        = 3,
		_NO_OF_COLUMNS = 4
	};

public:
	explicit HostCacheTableModel(QObject* parent = NULL, QWidget* container = NULL);
	~HostCacheTableModel();

	int rowCount(const QModelIndex& parent = QModelIndex()) const;
	int columnCount(const QModelIndex& parent) const;
	QVariant data(const QModelIndex& index, int nRole) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;

	void sort(int column, Qt::SortOrder order);

	int find(quint32 nHostID);

	HostData* dataFromRow(int nRow) const;
	//Rule* ruleFromIndex(const QModelIndex& index) const;

	void completeRefresh();

	void triggerHostRemoval(int nIndex);

public slots:
	void recieveHostInfo(SharedHostPtr pHost);

	/**
	 * @brief addHost adds a rule to the GUI.
	 * @param pHost : the host
	 */
	void addHost(SharedHostPtr pHost);

	/**
	 * @brief removeHost removes a host from the table model.
	 * This is to be triggered from the host cache AFTER the host has been removed.
	 * @param pHost : the host
	 */
	void removeHost(SharedHostPtr pHost);

	/**
	 * @brief updateHost updates the GUI for a specified host.
	 * @param nHostID : the ID of the host
	 */
	void updateHost(quint32 nHostID);

	/**
	 * @brief updateAll updates all hosts in the GUI.
	 */
	void updateAll();

private:
	void insert(HostData* pData);
	void erase(int nPos);
};

typedef HostCacheTableModel::HostData    HostData;

#endif // G2CACHETABLEMODEL_H