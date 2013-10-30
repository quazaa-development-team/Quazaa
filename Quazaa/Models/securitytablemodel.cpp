/*
** securitytablemodel.cpp
**** Copyright © Quazaa Development Team, 2009-2012.
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

#include "securitytablemodel.h"

#include "debug_new.h"

CSecurityTableModel::Rule::Rule(CSecureRule* pRule, CSecurityTableModel* model) : m_pRule(pRule)
{
#ifdef _DEBUG
	Q_ASSERT( pRule );
#endif // _DEBUG

	m_sContent		= m_pRule->getContentString();
	m_nAction		= m_pRule->m_nAction;
	m_bForever		= m_pRule->isForever();
	m_tExpire		= m_pRule->getExpiryTime();
	m_nToday		= m_pRule->getTodayCount();
	m_nTotal		= m_pRule->getTotalCount();
	m_sComment		= m_pRule->m_sComment;
	m_bAutomatic	= m_pRule->m_bAutomatic;
	m_nType			= m_pRule->type();

	switch( m_nAction )
	{
	case RuleAction::None:
		m_piAction = model->m_pIcons[0];
		break;

	case RuleAction::Accept:
		m_piAction = model->m_pIcons[1];
		break;

	case RuleAction::Deny:
		m_piAction = model->m_pIcons[2];
		break;

	default:
		Q_ASSERT( false );
	}
}

CSecurityTableModel::Rule::~Rule()
{

}

/** Requires an existing security manager read lock **/
bool CSecurityTableModel::Rule::update(int row, int col, QModelIndexList &to_update,
									   CSecurityTableModel *model)
{
	Q_ASSERT( m_pRule );

	bool bReturn = false;

	if(m_pRule->isBeingRemoved())
		return bReturn;

	if ( m_sContent != m_pRule->getContentString() )
	{
		to_update.append( model->index( row, CONTENT ) );
		m_sContent = m_pRule->getContentString();

		if ( col == CONTENT )
			bReturn = true;
	}

	if ( m_nType != m_pRule->type() )
	{
		to_update.append( model->index( row, TYPE ) );
		m_nType = m_pRule->type();

		if ( col == TYPE )
			bReturn = true;
	}

	if ( m_nAction != m_pRule->m_nAction )
	{
		to_update.append( model->index( row, ACTION ) );
		m_nAction = m_pRule->m_nAction;

		switch( m_nAction )
		{
		case RuleAction::None:
			m_piAction = model->m_pIcons[0];
			break;

		case RuleAction::Accept:
			m_piAction = model->m_pIcons[1];
			break;

		case RuleAction::Deny:
			m_piAction = model->m_pIcons[2];
			break;

		default:
			Q_ASSERT( false );
		}

		if ( col == ACTION )
			bReturn = true;
	}

	if ( m_tExpire != m_pRule->getExpiryTime() )
	{
		to_update.append( model->index( row, EXPIRES ) );
		m_tExpire = m_pRule->getExpiryTime();

		if ( col == EXPIRES )
			bReturn = true;
	}

	if ( m_bForever != m_pRule->isForever() )
	{
		to_update.append( model->index( row, EXPIRES ) );
		m_bForever = m_pRule->isForever();

		if ( col == EXPIRES )
			bReturn = true;
	}

	if ( m_nToday != m_pRule->getTodayCount() )
	{
		to_update.append( model->index( row, HITS ) );
		m_nToday = m_pRule->getTodayCount();
		m_nTotal = m_pRule->getTotalCount();

		if ( col == HITS )
			bReturn = true;
	}

	if ( m_sComment != m_pRule->m_sComment )
	{
		to_update.append( model->index( row, COMMENT ) );
		m_sComment = m_pRule->m_sComment;

		if ( col == COMMENT )
			bReturn = true;
	}

	m_bAutomatic = m_pRule->m_bAutomatic;

	return bReturn;
}

QVariant CSecurityTableModel::Rule::data(int col) const
{
	switch ( col )
	{
		case CONTENT:
			return m_sContent;

		case TYPE:
			switch (m_nType)
			{
				case RuleType::IPAddress:
					return tr("IP Address");
				case RuleType::IPAddressRange:
					return tr("IP Address Range");
				case RuleType::Hash:
					return tr("File Filter");
				case RuleType::RegularExpression:
					return tr("Regular Expression");
				case RuleType::UserAgent:
					return tr("User Agent");
				case RuleType::Content:
					return tr("Content Filter");
				default:
					return tr("Unknown");
			}

		case ACTION:
			return actionToString( m_nAction );

		case EXPIRES:
			return expiryToString( m_tExpire );

		case HITS:
			return QString( "%1 (%2)" ).arg( QString::number( m_nToday ),
											 QString::number( m_nTotal ) );

		case COMMENT:
			return m_sComment;
	}

	return QVariant();
}

bool CSecurityTableModel::Rule::lessThan(int col,
										 const CSecurityTableModel::Rule* const pOther) const
{
	if ( !pOther )
		return false;

	switch ( col )
	{
	case CONTENT:
		return m_sContent < pOther->m_sContent;

	case TYPE:
		return m_nType < pOther->m_nType;

	case ACTION:
		return m_nAction  < pOther->m_nAction;

	case EXPIRES:
		if(!m_bForever && pOther->m_bForever)
			return true;
		return m_tExpire  < pOther->m_tExpire;

	case HITS:
		return m_nTotal   < pOther->m_nTotal;

	case COMMENT:
		return m_sComment < pOther->m_sComment;

	default:
		return false;
	}

}

QString CSecurityTableModel::Rule::actionToString(RuleAction::Action nAction) const
{
	switch( nAction )
	{
	case RuleAction::None:
		return tr( "None" );

	case RuleAction::Accept:
		return tr( "Allow" );

	case RuleAction::Deny:
		return tr( "Deny" );
	}

	return QString();
}

// TODO: Implement loading translation string.
QString CSecurityTableModel::Rule::expiryToString(quint32 tExpire) const
{
	if(tExpire == RuleTime::Special) {
		if(m_bForever)
			return tr( "Forever" );
		else
			return tr( "Session" );
	}

	return QDateTime::fromTime_t( tExpire ).toLocalTime().toString();
}

CSecurityTableModel::CSecurityTableModel(QObject* parent, QWidget* container) :
	QAbstractTableModel( parent ),
	m_oContainer( container ),
	m_nSortColumn( -1 ),
	m_bNeedSorting( false )
{
	m_pIcons[0] = new QIcon( ":/Resource/Security/Null.ico" );
	m_pIcons[1] = new QIcon( ":/Resource/Security/Accept.ico" );
	m_pIcons[2] = new QIcon( ":/Resource/Security/Deny.ico" );

	connect( &securityManager, SIGNAL( ruleAdded( CSecureRule* ) ), this,
			 SLOT( addRule( CSecureRule* ) ), Qt::QueuedConnection );

	connect( &securityManager, SIGNAL( ruleRemoved( CSecureRule* ) ), this,
			 SLOT( removeRule( CSecureRule* ) ), Qt::QueuedConnection );

	// This handles GUI updates on rule statistics changes.
	connect( &securityManager, SIGNAL( securityHit() ), this,
			 SLOT( updateAll() ), Qt::QueuedConnection );

	// This needs to be called to make sure that all rules added to the securityManager before this
	// part of the GUI is loaded are properly added to the model.
	completeRefresh();
}

CSecurityTableModel::~CSecurityTableModel()
{
	qDeleteAll( m_lNodes );
	m_lNodes.clear();

	delete m_pIcons[0];
	delete m_pIcons[1];
	delete m_pIcons[2];
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
			return *pRule->m_piAction;
		}
	}
	// TODO: Reimplement formatting options in models.
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
			return tr( "Content" );

		case TYPE:
			return tr( "Type" );

		case ACTION:
			return tr( "Action" );

		case EXPIRES:
			return tr( "Expires" );

		case HITS:
			return tr( "Hits" );

		case COMMENT:
			return tr( "Comment" );
		}
	}
	else if ( role == Qt::ToolTipRole )
	{
		switch(section)
		{
		case CONTENT:
			return tr( "The content of the Security Manager rule" );

		case TYPE:
			return tr( "The rule type." );

		case ACTION:
			return tr( "Whether a rule blocks or allows content" );

		case EXPIRES:
			return tr( "When the Security Manager rule expires" );

		case HITS:
			return tr( "How often the rule has been hit today (since its creation date)" );

		case COMMENT:
			return tr( "Comment" );
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

void CSecurityTableModel::sort(int column, Qt::SortOrder order)
{
	m_nSortColumn = column;
	m_nSortOrder  = order;

	emit layoutAboutToBeChanged();

	// I hope this is the correct way to convince Qt...
	QModelIndexList oldIdx = persistentIndexList();
	QModelIndexList newIdx = oldIdx;

	qStableSort( m_lNodes.begin(), m_lNodes.end(), CSecurityTableModelCmp( column, order ) );

	for ( int i = 0; i < oldIdx.size(); ++i ) // For each persistent index
	{
		int oldRow = oldIdx.at(i).row();

		// if oldRow is outside range
		if ( oldRow > m_lNodes.size()
				// or the index points to another item
				|| oldIdx.at(i).internalPointer() != m_lNodes.at(oldRow) )
		{
			// find the correct item and update persistent index
			for ( int j = 0; j < m_lNodes.size(); ++j )
			{
				if ( oldIdx.at(i).internalPointer() == m_lNodes.at(j) )
				{
					newIdx[i] = createIndex( j, oldIdx.at(i).column(),
											 oldIdx.at(i).internalPointer() );
					break;
				}
			}
		}
	}

	changePersistentIndexList( oldIdx, newIdx );
	emit layoutChanged();
}

CSecureRule* CSecurityTableModel::ruleFromIndex(const QModelIndex &index)
{
	if ( index.isValid() && index.row() < m_lNodes.count() && index.row() >= 0 )
		return m_lNodes[ index.row() ]->m_pRule;
	else
		return NULL;
}

void CSecurityTableModel::completeRefresh()
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
	connect( &securityManager, SIGNAL( ruleInfo( CSecureRule* ) ), this,
			 SLOT( addRule( CSecureRule* ) ), Qt::QueuedConnection );

	// Request getting them back from the Security Manager.
	securityManager.requestRuleList();
}

void CSecurityTableModel::addRule(CSecureRule* pRule)
{
	if ( securityManager.check( pRule ) )
	{
		beginInsertRows( QModelIndex(), m_lNodes.size(), m_lNodes.size() );
		m_lNodes.append( new Rule( pRule, this ) );
		endInsertRows();
		m_bNeedSorting = true;
	}

	// We should probably be the only one listening.
	if ( securityManager.receivers ( CSecurity::ruleInfoSignal ) )
	{
		// Make sure we don't recieve any signals we don't want once we got all rules once.
		if ( m_lNodes.size() == (int)securityManager.getCount() )
			disconnect( &securityManager, SIGNAL( ruleInfo( CSecureRule* ) ),
						this, SLOT( addRule( CSecureRule* ) ) );

#ifdef _DEBUG
		Q_ASSERT( m_lNodes.size() <= (int)securityManager.getCount() );
#endif // _DEBUG
	}
}

void CSecurityTableModel::removeRule(CSecureRule* pRule)
{
	for ( int i = 0; i < m_lNodes.size(); i++ )
	{
		if ( m_lNodes[i]->m_pRule == pRule )
		{
			beginRemoveRows( QModelIndex(), i, i );
			delete m_lNodes[i];
			m_lNodes.removeAt( i );
			endRemoveRows();
			m_bNeedSorting = true;
			if(!pRule->isBeingRemoved())
				securityManager.remove(pRule);
			else
				pRule->deleteLater();
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
		m_bNeedSorting = false;
	}
	else
	{
		if ( !uplist.isEmpty() )
		{
			QAbstractItemView* pView = qobject_cast< QAbstractItemView* >( m_oContainer );

			if( pView )
			{
				foreach ( QModelIndex index, uplist )
				{
					pView->update( index );
				}
			}
		}
	}
}
