/*
** discoverytablemodel.cpp
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

#include <QAbstractItemView>

#include "discoverytablemodel.h"

#ifdef _DEBUG
#include "debug_new.h"
#endif

CDiscoveryTableModel::Service::Service(const CDiscoveryService* pService)
{
#ifdef _DEBUG
	Q_ASSERT( pService );
#endif // _DEBUG

//	QWriteLocker w( &discoveryManager.m_pRWLock );
	m_pNode = pService;

	// This makes sure that if pService is deleted within the Discovery Services Manager,
	// m_pNode is correctly set to NULL. Note that a write lock is required here.
	m_pNode->registerPointer( &m_pNode );

	m_sURL			= m_pNode->url();
	m_nType			= m_pNode->serviceType();
	m_nLastHosts	= m_pNode->lastHosts();
	m_nTotalHosts	= m_pNode->totalHosts();

	switch( m_nType )
	{
	case Discovery::stNull:
		Q_ASSERT( false ); // Should not happen.
		break;
	/*case Discovery::CDiscoveryService::stMulti:
		// m_iType = QIcon( ":/Resource/Discovery/Multi.ico" );
		break;*/
	case Discovery::stGWC:
		// m_iType = QIcon( ":/Resource/Discovery/GWC.ico" );
		break;
	default:
		Q_ASSERT( false );
	}
}

CDiscoveryTableModel::Service::~Service()
{
	//QWriteLocker w( &discoveryManager.m_pRWLock );
	// This is important to avoid memory access errors within the Discovery Services Manager.
	if ( m_pNode )
		m_pNode->unRegisterPointer( &m_pNode );
}

bool CDiscoveryTableModel::Service::update(int row, int col, QModelIndexList &to_update,
										   CDiscoveryTableModel *model)
{
	//QReadLocker l( &discoveryManager.m_pRWLock );

	if ( !m_pNode )
	{

#ifdef _DEBUG
	// We should have been informed about this event.
	Q_ASSERT( false );
#endif // _DEBUG

		return false;
	}

#ifdef _DEBUG
	//l.unlock();

	// pNode should be set to NULL on deletion of the rule object it points to.
	Q_ASSERT( discoveryManager.check( m_pNode ) );

	//l.relock();
#endif // _DEBUG

	bool bReturn = false;

	if ( m_nType != m_pNode->serviceType() )
	{
		to_update.append( model->index( row, TYPE ) );
		m_nType = m_pNode->serviceType();

		switch( m_nType )
		{
		case Discovery::stNull:
			Q_ASSERT( false ); // Should not happen.
			break;
		/*case Discovery::stMulti:
			// m_iType = QIcon( ":/Resource/Discovery/Multi.ico" );
			break;*/
		case Discovery::stGWC:
			// m_iType = QIcon( ":/Resource/Discovery/GWC.ico" );
			break;
		default:
			Q_ASSERT( false );
		}

		if ( col == TYPE )
			bReturn = true;
	}

	if ( m_sURL != m_pNode->url() )
	{
		to_update.append( model->index( row, URL ) );
		m_sURL = m_pNode->url();

		if ( col == URL )
			bReturn = true;
	}

	if ( m_nLastHosts != m_pNode->lastHosts() )
	{
		to_update.append( model->index( row, HOSTS ) );
		m_nLastHosts = m_pNode->lastHosts();

		if ( col == HOSTS )
			bReturn = true;
	}

	if ( m_nTotalHosts != m_pNode->totalHosts() )
	{
		to_update.append( model->index( row, TOTAL_HOSTS ) );
		m_nTotalHosts = m_pNode->totalHosts();

		if ( col == TOTAL_HOSTS )
			bReturn = true;
	}

	return bReturn;
}

QVariant CDiscoveryTableModel::Service::data(int col) const
{
	switch ( col )
	{
		case TYPE:
			return QString();
		case URL:
			return m_sURL;
		case HOSTS:
			return QString( m_nLastHosts );
		case TOTAL_HOSTS:
			return QString( m_nTotalHosts );
	}

	return QVariant();
}

bool CDiscoveryTableModel::Service::lessThan(int col, const CDiscoveryTableModel::Service* const pOther) const
{
	if ( !pOther )
		return false;

	switch ( col )
	{
	case TYPE:
		return m_nType < pOther->m_nType;
	case URL:
		return m_sURL  < pOther->m_sURL;
	case HOSTS:
		return m_nLastHosts < pOther->m_nLastHosts;
	case TOTAL_HOSTS:
		return m_nTotalHosts < pOther->m_nTotalHosts;
	default:
		return false;
	}

}

CDiscoveryTableModel::CDiscoveryTableModel(QObject *parent, QWidget* container) :
	QAbstractTableModel( parent ),
	m_oContainer( container ),
	m_nSortColumn( -1 ),
	m_bNeedSorting( false )
{
	/*connect( &discoveryManager, SIGNAL( serviceAdded( const CDiscoveryService* ) ), this,
			 SLOT( addService( const CDiscoveryService* ) ), Qt::QueuedConnection );

	connect( &discoveryManager, SIGNAL( serviceRemoved( QSharedPointer<CDiscoveryService> ) ), this,
			 SLOT( removeService( QSharedPointer<CDiscoveryService> ) ), Qt::QueuedConnection );*/

	// This needs to be called to make sure that all rules added to the discoveryManager before this
	// part of the GUI is loaded are properly added to the model.
	completeRefresh();
}

CDiscoveryTableModel::~CDiscoveryTableModel()
{
	qDeleteAll( m_lNodes );
	m_lNodes.clear();
}

int CDiscoveryTableModel::rowCount(const QModelIndex& parent) const
{
	if ( parent.isValid() )
	{
		return 0;
	}
	else
	{
		return m_lNodes.count();
	}
}

int CDiscoveryTableModel::columnCount(const QModelIndex& parent) const
{
	if ( parent.isValid() )
	{
		return 0;
	}
	else
	{
		return _NO_OF_COLUMNS;
	}
}

QVariant CDiscoveryTableModel::data(const QModelIndex& index, int role) const
{
	if ( !index.isValid() || index.row() > m_lNodes.size() || index.row() < 0 )
	{
		return QVariant();
	}

	const Service* pService = m_lNodes.at( index.row() );

	if ( role == Qt::DisplayRole )
	{
		return pService->data( index.column() );
	}
	else if ( role == Qt::DecorationRole )
	{
		if ( index.column() == TYPE )
		{
			return pService->m_iType;
		}
	}
	/*else if ( role == Qt::ForegroundRole )
	{
		switch ( nbr->nState )
		{
			case nsConnected:
				//return skinSettings.listsColorSpecial;
				break;
			case nsConnecting:
				//return skinSettings.listsColorActive;
				break;
			default:
				//return skinSettings.listsColorNormal;
				break;
		}
	}*/
	/*else if ( role == Qt::FontRole )
	{
		QFont font = qApp->font(m_oContainer);
		switch ( nbr->nState )
		{
			case nsConnected:
				//font.setWeight(skinSettings.listsWeightSpecial);
				return font;
				break;
			case nsConnecting:
				//font.setWeight(skinSettings.listsWeightActive);
				return font;
				break;
			default:
				//font.setWeight(skinSettings.listsWeightNormal);
				return font;
				break;
		}
	}*/

	return QVariant();
}

QVariant CDiscoveryTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if ( orientation != Qt::Horizontal )
		return QVariant();

	if ( role == Qt::DisplayRole )
	{
		switch ( section )
		{
		case TYPE:
			return tr("Type");
		case URL:
			return tr("URL");
		case HOSTS:
			return tr("Hosts");
		case TOTAL_HOSTS:
			return tr("Total Hosts");
		}
	}
	else if ( role == Qt::ToolTipRole )
	{
		switch( section )
		{
		case TYPE:
			return tr("The type of the Discovery Service");
		case URL:
			return tr("The URL of the Discovery Service");
		case HOSTS:
			return tr("The number of hosts returned the last time this service was queried");
		case TOTAL_HOSTS:
			return tr("The total number of hosts returned from this service");
		}
	}

	return QVariant();
}

QModelIndex CDiscoveryTableModel::index(int row, int column, const QModelIndex &parent) const
{
	if ( parent.isValid() || row < 0 || row >= m_lNodes.count() )
		return QModelIndex();
	else
		return createIndex( row, column, m_lNodes[row] );
}

class CDiscoveryTableModelCmp
{
public:
	CDiscoveryTableModelCmp( int col, Qt::SortOrder o ) :
		column( col ),
		order( o )
	{
	}

	bool operator()( CDiscoveryTableModel::Service* a, CDiscoveryTableModel::Service* b )
	{
		if ( order == Qt::AscendingOrder )
		{
			return a->lessThan( column, b );
		}
		else
		{
			return b->lessThan( column, a );
		}
	}

	int column;
	Qt::SortOrder order;
};

void CDiscoveryTableModel::sort(int column, Qt::SortOrder order)
{
	m_nSortColumn = column;
	m_nSortOrder = order;

	emit layoutAboutToBeChanged();
	qStableSort( m_lNodes.begin(), m_lNodes.end(), CDiscoveryTableModelCmp( column, order ) );
	emit layoutChanged();
}

const Discovery::CDiscoveryService* CDiscoveryTableModel::nodeFromRow(quint32 row) const
{
	if ( row < (quint32)m_lNodes.count() )
	{
		return m_lNodes[ row ]->m_pNode;
	}
	else
	{
		return NULL;
	}
}

const Discovery::CDiscoveryService* CDiscoveryTableModel::nodeFromIndex(const QModelIndex &index) const
{
	if ( !index.isValid() || !( index.row() < m_lNodes.count() ) || index.row() < 0 )
		return NULL;
	else
		return m_lNodes[ index.row() ]->m_pNode;
}

void CDiscoveryTableModel::completeRefresh()
{
	// Remove all rules.
	if ( m_lNodes.size() )
	{
		beginRemoveRows( QModelIndex(), 0, m_lNodes.size() - 1 );
		for ( int i = 0; i < m_lNodes.size(); ++i )
		{
			delete m_lNodes[i];
		}
		m_lNodes.clear();
		endRemoveRows();
	}

	// Note that this slot is automatically disconnected once all rules have been recieved once.
	connect( &discoveryManager, SIGNAL( serviceInfo( const CDiscoveryService* ) ), this,
			 SLOT( addService(const CDiscoveryService* ) ), Qt::QueuedConnection );

	// Request getting them back from the Security Manager.
	discoveryManager.requestServiceList();
}

void CDiscoveryTableModel::addService( const CDiscoveryTableModel::CDiscoveryService* pService)
{
	if ( discoveryManager.check( pService ) )
	{
		beginInsertRows( QModelIndex(), m_lNodes.size(), m_lNodes.size() );
		m_lNodes.append( new Service( pService ) );
		endInsertRows();

		m_bNeedSorting = true;
	}

//	QReadLocker l( &discoveryManager.m_pRWLock );

	// Make sure we don't recieve any signals we don't want once we got all rules once.
	if ( m_lNodes.size() == (int)discoveryManager.count() )
		disconnect( &discoveryManager, SIGNAL( serviceInfo( const CDiscoveryService* ) ),
					this, SLOT( addService( const CDiscoveryService* ) ) );
}

void CDiscoveryTableModel::removeService(const QSharedPointer<CDiscoveryService> pService)
{
	for ( quint32 i = 0, nMax = m_lNodes.size(); i < nMax; i++ )
	{
		if ( /* *(m_lNodes[i]->m_pNode) == *pService*/ true )
		{
			beginRemoveRows( QModelIndex(), i, i );
			delete m_lNodes[i];
			m_lNodes.remove( i, 1 );
			endRemoveRows();
			m_bNeedSorting = true;
			break;
		}
	}
}

void CDiscoveryTableModel::updateAll()
{
	{
//		QReadLocker l( &(discoveryManager.m_pRWLock) );

		if ( (quint32)m_lNodes.size() != discoveryManager.count() )
		{
#ifdef _DEBUG
			// This is something that should not have happened.
			Q_ASSERT( false );
#endif // _DEBUG

			completeRefresh();
			return;
		}
	}

	QModelIndexList uplist;
	bool bSort = m_bNeedSorting;

	for ( quint32 i = 0, max = m_lNodes.count(); i < max; ++i )
	{
		if ( m_lNodes[i]->update( i, m_nSortColumn, uplist, this ) )
			bSort = true;
	}

	if ( bSort )
	{
		sort( m_nSortColumn, m_nSortOrder );
	}
	else
	{
		if ( !uplist.isEmpty() )
		{
			QAbstractItemView* pView = qobject_cast< QAbstractItemView* >( m_oContainer );

			if ( pView )
			{
				foreach ( QModelIndex index, uplist )
				{
					pView->update( index );
				}
			}
		}
	}
}

