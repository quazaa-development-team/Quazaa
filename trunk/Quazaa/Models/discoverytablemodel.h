#ifndef DISCOVERYTABLEMODEL_H
#define DISCOVERYTABLEMODEL_H

#include <QAbstractTableModel>
#include <QIcon>

#include "Discovery/discovery.h"

class CDiscoveryTableModel : public QAbstractTableModel
{
	Q_OBJECT

private:
	typedef Discovery::CDiscoveryService CDiscoveryService;

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
		// Object directly managed by security manager.
		CDiscoveryService*		m_pNode;

		QString					m_sURL;
		CDiscoveryService::ServiceType	m_nType;
		quint32					m_nLastHosts;
		quint32					m_nTotalHosts;
		QIcon					m_iType;

		Service(CDiscoveryService* pService);
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

	CDiscoveryService* nodeFromRow(quint32 row);
	//CDiscoveryService* nodeFromIndex(const QModelIndex& index);

	void completeRefresh();

public slots:
	void addService(CDiscoveryService* pService);
	void removeService(const QSharedPointer<CDiscoveryService> pService);
	void updateAll();

};

#endif // DISCOVERYTABLEMODEL_H
