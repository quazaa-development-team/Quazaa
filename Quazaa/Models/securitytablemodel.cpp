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

#include "securitytablemodel.h"
#include "Security/security.h"

#ifdef _DEBUG
#include "debug_new.h"
#endif

CSecurityTableModel::Rule::Rule(CSecureRule* pRule)
{
#ifdef _DEBUG
	Q_ASSERT( pRule );
#endif // _DEBUG

	QWriteLocker w( &securityManager.m_pRWLock );
	m_pNode = pRule;

	// This makes sure that if pRule is deleted within the Security Manager,
	// pNode is correctly set to NULL. Note that a write lock is required here.
	m_pNode->registerPointer( &m_pNode );

	m_sContent	= m_pNode->getContentString();
	m_nAction	= m_pNode->m_nAction;
	m_tExpire	= m_pNode->m_tExpire;
	m_nToday	= m_pNode->getTodayCount();
	m_nTotal	= m_pNode->getTotalCount();
	m_sComment	= m_pNode->m_sComment;

	switch( m_pNode->m_nAction )
	{
	case security::CSecureRule::srNull:
		m_iAction = QIcon( ":/Resource/Security/Null.ico" );
		break;
	case security::CSecureRule::srAccept:
		m_iAction = QIcon( ":/Resource/Security/Accept.ico" );
		break;
	case security::CSecureRule::srDeny:
		m_iAction = QIcon( ":/Resource/Security/Deny.ico" );
		break;
	default:
		Q_ASSERT( false );
	}
}

CSecurityTableModel::Rule::~Rule()
{
	QWriteLocker w( &securityManager.m_pRWLock );
	// This is important to avoid memory access errors within the Security Manager.
	if ( m_pNode )
		m_pNode->unRegisterPointer( &m_pNode );
}

bool CSecurityTableModel::Rule::update(int row, int col, QModelIndexList &to_update,
									   CSecurityTableModel *model)
{
	QReadLocker l( &securityManager.m_pRWLock );

	if ( !m_pNode )
	{

#ifdef _DEBUG
	// We should have been informed about this event.
	Q_ASSERT( false );
#endif // _DEBUG

		return false;
	}

#ifdef _DEBUG
	l.unlock();

	// pNode should be set to NULL on deletion of the rule object it points to.
	Q_ASSERT( securityManager.check( m_pNode ) );

	l.relock();
#endif // _DEBUG

	bool bReturn = false;

	if ( m_sContent != m_pNode->getContentString() )
	{
		to_update.append( model->index( row, CONTENT ) );
		m_sContent = m_pNode->getContentString();

		if ( col == CONTENT )
			bReturn = true;
	}

	if ( m_nAction != m_pNode->m_nAction )
	{
		to_update.append( model->index( row, ACTION ) );
		m_nAction = m_pNode->m_nAction;

		switch( m_nAction )
		{
		case security::CSecureRule::srNull:
			m_iAction = QIcon( ":/Resource/Security/Null.ico" );
			break;
		case security::CSecureRule::srAccept:
			m_iAction = QIcon( ":/Resource/Security/Accept.ico" );
			break;
		case security::CSecureRule::srDeny:
			m_iAction = QIcon( ":/Resource/Security/Deny.ico" );
			break;
		default:
			Q_ASSERT( false );
		}

		if ( col == ACTION )
			bReturn = true;
	}

	if ( m_tExpire != m_pNode->m_tExpire )
	{
		to_update.append( model->index( row, EXPIRES ) );
		m_tExpire = m_pNode->m_tExpire;

		if ( col == EXPIRES )
			bReturn = true;
	}

	if ( m_nToday != m_pNode->getTodayCount() )
	{
		to_update.append( model->index( row, HITS ) );
		m_nToday = m_pNode->getTodayCount();
		m_nTotal = m_pNode->getTotalCount();

		if ( col == HITS )
			bReturn = true;
	}

	if ( m_sComment != m_pNode->m_sComment )
	{
		to_update.append( model->index( row, COMMENT ) );
		m_sComment = m_pNode->m_sComment;

		if ( col == COMMENT )
			bReturn = true;
	}

	return bReturn;
}

QVariant CSecurityTableModel::Rule::data(int col) const
{
	switch ( col )
	{
		case CONTENT:
			return m_sContent;
		case ACTION:
			return actionToString( m_nAction );
		case EXPIRES:
			return expiryToString( m_tExpire );
		case HITS:
			return QString( "%1 (%2)" ).arg( QString::number( m_nToday ), QString::number( m_nTotal ) );
		case COMMENT:
			return m_sComment;
	}

	return QVariant();
}

bool CSecurityTableModel::Rule::lessThan(int col, const CSecurityTableModel::Rule* const pOther) const
{
	if ( !pOther )
		return false;

	switch ( col )
	{
	case CONTENT:
		return m_sContent < pOther->m_sContent;
	case ACTION:
		return m_nAction  < pOther->m_nAction;
	case EXPIRES:
		return m_tExpire  < pOther->m_tExpire;
	case HITS:
		return m_nTotal   < pOther->m_nTotal;
	case COMMENT:
		return m_sComment < pOther->m_sComment;
	default:
		return false;
	}

}

QString CSecurityTableModel::Rule::actionToString(CSecureRule::Policy nAction) const
{
	switch( nAction )
	{
	case CSecureRule::srNull:
		return "None";
	case CSecureRule::srAccept:
		return "Allow";
	case CSecureRule::srDeny:
		return "Deny";
	}

	return QString();
}

// TODO: Implement loading translation string.
QString CSecurityTableModel::Rule::expiryToString(quint32 tExpire) const
{
	switch( tExpire )
	{
	case CSecureRule::srIndefinite:
		return tr( "Never" );
	case CSecureRule::srSession:
		return tr( "Session" );
	}

	return QString().sprintf( "%.2u:%.2u:%.2u", tExpire / 3600,
							  (tExpire % 3600 / 60), (tExpire % 3600) % 60 );
}

CSecurityTableModel::CSecurityTableModel(QObject* parent, QWidget* container) :
	QAbstractTableModel( parent )
{
	m_oContainer = container;
	m_nSortColumn = -1;
	m_bNeedSorting = false;

	connect( &securityManager, SIGNAL( ruleAdded( CSecureRule* ) ), this,
			 SLOT( addRule( CSecureRule* ) ), Qt::QueuedConnection );

	connect( &securityManager, SIGNAL( ruleRemoved( QSharedPointer<CSecureRule> ) ), this,
			 SLOT( removeRule( QSharedPointer<CSecureRule> ) ), Qt::QueuedConnection );

	// This needs to be called to make sure that all rules added to the securityManager before this
	// part of the GUI is loaded are properly added to the model.
	retrieveAllRules();
}

CSecurityTableModel::~CSecurityTableModel()
{
	qDeleteAll( m_lNodes );
	m_lNodes.clear();
}

int CSecurityTableModel::rowCount(const QModelIndex& parent) const
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

int CSecurityTableModel::columnCount(const QModelIndex& parent) const
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

QVariant CSecurityTableModel::data(const QModelIndex& index, int role) const
{
	if ( !index.isValid() || index.row() > m_lNodes.size() || index.row() < 0 )
	{
		return QVariant();
	}

	const Rule* pRule = m_lNodes.at( index.row() );

	if ( role == Qt::DisplayRole )
	{
		return pRule->data( index.column() );
	}
	else if ( role == Qt::DecorationRole )
	{
		if ( index.column() == ACTION )
		{
			return pRule->m_iAction;
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

QVariant CSecurityTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if ( orientation != Qt::Horizontal )
		return QVariant();

	if ( role == Qt::DisplayRole )
	{
		switch ( section )
		{
		case CONTENT:
			return tr("Content");
		case ACTION:
			return tr("Action");
		case EXPIRES:
			return tr("Expires");
		case HITS:
			return tr("Hits");
		case COMMENT:
			return tr("Comment");
		}
	}
	else if(role == Qt::ToolTipRole)
	{
		switch(section)
		{
		case CONTENT:
			return tr("The content of the Security Manager rule");
		case ACTION:
			return tr("Whether a rule blocks or allows content");
		case EXPIRES:
			return tr("When the Security Manager rule expires");
		case HITS:
			return tr("How often the rule has been hit today (since its creation date)");
		case COMMENT:
			return tr("Comment");
		}
	}

	return QVariant();
}

QModelIndex CSecurityTableModel::index(int row, int column, const QModelIndex &parent) const
{
	if ( parent.isValid() || row < 0 || row >= m_lNodes.count() )
		return QModelIndex();
	else
		return createIndex( row, column, m_lNodes[row] );
}

class CSecurityTableModelCmp
{
public:
	CSecurityTableModelCmp( int col, Qt::SortOrder o ) :
		column( col ),
		order( o )
	{
	}

	bool operator()( CSecurityTableModel::Rule* a, CSecurityTableModel::Rule* b )
	{
		if( order == Qt::AscendingOrder )
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

void CSecurityTableModel::sort(int column, Qt::SortOrder order)
{
	m_nSortColumn = column;
	m_nSortOrder = order;

	emit layoutAboutToBeChanged();
	qStableSort( m_lNodes.begin(), m_lNodes.end(), CSecurityTableModelCmp( column, order ) );
	emit layoutChanged();
}

security::CSecureRule* CSecurityTableModel::nodeFromIndex(const QModelIndex &index)
{
	if ( index.isValid() && index.row() < m_lNodes.count() && index.row() >= 0 )
		return m_lNodes[ index.row() ]->m_pNode;
	else
		return NULL;
}

void CSecurityTableModel::addRule(CSecureRule* pRule)
{
	if ( securityManager.check( pRule ) )
	{
		beginInsertRows( QModelIndex(), m_lNodes.size(), m_lNodes.size() );
		m_lNodes.append( new Rule( pRule ) );
		endInsertRows();
		m_bNeedSorting = true;
	}

	// We should probably be the only one listening.
	if ( QObject::receivers ( SIGNAL( ruleInfo( CSecureRule* ) ) ) )
	{
		QReadLocker l( &(securityManager.m_pRWLock) );

		// Make sure we don't recieve any signals we don't want once we got all rules once.
		if ( m_lNodes.size() == (int)securityManager.getCount() )
			disconnect( &securityManager, SIGNAL( ruleInfo( CSecureRule* ) ),
						this, SLOT( addRule( CSecureRule* ) ) );
	}
}

void CSecurityTableModel::removeRule(const QSharedPointer<CSecureRule> pRule)
{
	for ( quint32 i = 0, nMax = m_lNodes.size(); i < nMax; i++ )
	{
		if ( *(m_lNodes[i]->m_pNode) == *pRule )
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

void CSecurityTableModel::updateAll()
{
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
			foreach ( QModelIndex index, uplist )
			{
				// TODO: question: is there a reason for this line being inside the for each loop?
				// couldn't this be moved before the loop and the loop be only called if this
				// returns != NULL?
				QAbstractItemView* pView = qobject_cast< QAbstractItemView* >( m_oContainer );

				if ( pView )
				{
					pView->update( index );
				}
			}
		}
	}
}

void CSecurityTableModel::retrieveAllRules()
{
	// Remove all rules.
	if ( m_lNodes.size() )
	{
		beginRemoveRows( QModelIndex(), 0, m_lNodes.size() - 1 );
		qDeleteAll( m_lNodes );
		m_lNodes.clear();
		endRemoveRows();
	}

	// Note that this slot is automatically disconnected once all rules have been recieved once.
	connect( &securityManager, SIGNAL( ruleInfo( CSecureRule* ) ), this,
			 SLOT( addRule( CSecureRule* ) ), Qt::QueuedConnection );

	// Request getting them back from the Security Manager.
	securityManager.requestRuleList();
}
