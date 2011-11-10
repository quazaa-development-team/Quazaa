/*
** $Id$
**
** Copyright © Quazaa Development Team, 2009-2011.
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

CSecurityTableModel::Rule::Rule(const CSecureRule* const pRule)
{
#ifdef _DEBUG
	Q_ASSERT( pRule );
#endif // _DEBUG

	this->pNode	= pRule->getCopy();

	sContent	= pRule->getContentString();
	nAction		= pRule->m_nAction;
	tExpire		= pRule->m_tExpire;
	nToday		= pRule->getTodayCount();
	nTotal		= pRule->getTotalCount();
	sComment	= pRule->m_sComment;

	switch( pRule->m_nAction )
	{
	case security::CSecureRule::srNull:
		//iAction = QIcon( ":/Resource/Security/Null.ico" );
		break;
	case security::CSecureRule::srAccept:
		//iAction = QIcon( ":/Resource/Security/Accept.ico" );
		break;
	case security::CSecureRule::srDeny:
		//iAction = QIcon( ":/Resource/Security/Deny.ico" );
		break;
	default:
		Q_ASSERT( false );
	}
}

bool CSecurityTableModel::Rule::update(int row, int col, QModelIndexList &to_update, CSecurityTableModel *model)
{
	if ( !securityManager.check( pNode ) )
		return false;

	bool bRet = false;

	if ( sContent != pNode->getContentString() )
	{
		to_update.append( model->index( row, CONTENT ) );
		sContent = pNode->getContentString();

		if( col == CONTENT )
			bRet = true;
	}

	if ( nAction != pNode->m_nAction )
	{
		to_update.append( model->index( row, ACTION ) );
		nAction = pNode->m_nAction;

		switch( nAction )
		{
		case security::CSecureRule::srNull:
			iAction = QIcon( ":/Resource/Security/Null.ico" );
			break;
		case security::CSecureRule::srAccept:
			iAction = QIcon( ":/Resource/Security/Accept.ico" );
			break;
		case security::CSecureRule::srDeny:
			iAction = QIcon( ":/Resource/Security/Deny.ico" );
			break;
		default:
			Q_ASSERT( false );
		}

		if ( col == ACTION )
			bRet = true;
	}

	if ( tExpire != pNode->m_tExpire )
	{
		to_update.append( model->index( row, EXPIRES ) );
		tExpire = pNode->m_tExpire;

		if ( col == EXPIRES )
			bRet = true;
	}

	if ( nToday != pNode->getTodayCount() )
	{
		to_update.append( model->index( row, HITS ) );
		nToday = pNode->getTodayCount();
		nTotal = pNode->getTotalCount();

		if ( col == HITS )
			bRet = true;
	}

	if ( sComment != pNode->m_sComment )
	{
		to_update.append( model->index( row, COMMENT ) );
		sComment = pNode->m_sComment;

		if ( col == COMMENT )
			bRet = true;
	}

	return bRet;
}

QVariant CSecurityTableModel::Rule::data(int col) const
{
	switch ( col )
	{
		case CONTENT:
			return sContent;
		case ACTION:
			return actionToString( nAction );
		case EXPIRES:
			return expiryToString( tExpire );
		case HITS:
			return QString( "%1 (%2)" ).arg( QString::number( nToday ), QString::number( nTotal ) );
		case COMMENT:
			return sComment;
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
		return sContent < pOther->sContent;
	case ACTION:
		return nAction  < pOther->nAction;
	case EXPIRES:
		return tExpire  < pOther->tExpire;
	case HITS:
		return nTotal   < pOther->nTotal;
	case COMMENT:
		return sComment < pOther->sComment;
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

	return QString().sprintf("%.2u:%.2u:%.2u", tExpire / 3600, (tExpire % 3600 / 60), (tExpire % 3600) % 60);
}

CSecurityTableModel::CSecurityTableModel(QObject* parent, QWidget* container) :
	QAbstractTableModel( parent )
{
	m_oContainer = container;
	m_nSortColumn = -1;
	m_bNeedSorting = false;

	connect( &securityManager, SIGNAL( ruleAdded(   QSharedPointer<CSecureRule> ) ), this, SLOT( addRule(    QSharedPointer<CSecureRule> ) ), Qt::QueuedConnection );
	connect( &securityManager, SIGNAL( ruleRemoved( QSharedPointer<CSecureRule> ) ), this, SLOT( removeRule( QSharedPointer<CSecureRule> ) ), Qt::QueuedConnection );
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
			return pRule->iAction;
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
	if( orientation != Qt::Horizontal )
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
		return m_lNodes[ index.row() ]->pNode;
	else
		return NULL;
}

void CSecurityTableModel::addRule(const QSharedPointer<CSecureRule> pRule)
{
	if ( securityManager.check( pRule.data() ) )
	{
		beginInsertRows( QModelIndex(), m_lNodes.size(), m_lNodes.size() );
		m_lNodes.append( new Rule( pRule.data() ) );
		endInsertRows();
		m_bNeedSorting = true;
	}
}

void CSecurityTableModel::removeRule(const QSharedPointer<CSecureRule> pRule)
{
	for ( quint32 i = 0, nMax = m_lNodes.size(); i < nMax; i++ )
	{
		if ( m_lNodes[i]->pNode == pRule )
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
				// todo: question: is there a reason for this line being inside the for each loop?
				// couldn't this be moved before the loop and the loop be only called if this returns != NULL?
				QAbstractItemView* pView = qobject_cast< QAbstractItemView* >( m_oContainer );

				if ( pView )
				{
					pView->update( index );
				}
			}
		}
	}
}

