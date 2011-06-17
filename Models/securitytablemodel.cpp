#include "securitytablemodel.h"
#include "Security/security.h"

CSecurityTableModel::Rule::Rule(CSecureRule* pRule)
{
	this->pRule	= pRule->getCopy();

	sContent	= pRule->getContentString();
	nAction		= pRule->m_nAction;
	tExpire		= pRule->m_tExpire;
	nToday		= pRule->getTodayCount();
	nTotal		= pRule->getTotalCount();
	sComment	= pRule->m_sComment;

	switch( pRule->m_nAction )
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
}

bool CSecurityTableModel::Rule::update(int row, int col, QModelIndexList &to_update, CSecurityTableModel *model)
{
	if( !Security.check( pRule ) )
		return false;

	bool bRet = false;

	if( sContent != pRule->getContentString() )
	{
		to_update.append( model->index( row, CONTENT ) );
		sContent = pRule->getContentString();

		if( col == CONTENT )
			bRet = true;
	}

	if( nAction != pRule->m_nAction )
	{
		to_update.append( model->index( row, ACTION ) );
		nAction = pRule->m_nAction;

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

		if( col == ACTION )
			bRet = true;
	}

	if( tExpire != pRule->m_tExpire )
	{
		to_update.append( model->index( row, EXPIRES ) );
		tExpire = pRule->m_tExpire;

		if( col == EXPIRES )
			bRet = true;
	}

	if( nToday != pRule->getTodayCount() )
	{
		to_update.append( model->index( row, HITS ) );
		nToday = pRule->getTodayCount();
		nTotal = pRule->getTotalCount();

		if( col == HITS )
			bRet = true;
	}

	if( sComment != pRule->m_sComment )
	{
		to_update.append( model->index( row, COMMENT ) );
		sComment = pRule->m_sComment;

		if( col == COMMENT )
			bRet = true;
	}

	return bRet;
}

QVariant CSecurityTableModel::Rule::data(int col) const
{
	switch(col)
	{
		case CONTENT:
			return sContent;
		case ACTION:
			return actionToString( nAction );
		case EXPIRES:
			return expiryToString( tExpire );
		case HITS:
			return QString().sprintf("%.2u (%.2u)", nToday, nTotal);
		case COMMENT:
			return sComment;
	}

	return QVariant();
}

bool CSecurityTableModel::Rule::lessThan(int col, CSecurityTableModel::Rule *pOther) const
{
	switch(col)
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
		return "Never";
	case CSecureRule::srSession:
		return "Session";
	}

	return QString().sprintf("%.2u:%.2u:%.2u", tExpire / 3600, (tExpire % 3600 / 60), (tExpire % 3600) % 60);
}

CSecurityTableModel::CSecurityTableModel(QObject* parent, QWidget* container) :
	QAbstractTableModel( parent )
{
	m_oContainer = container;
	m_nSortColumn = -1;
	m_bNeedSorting = false;

	connect( &Security, SIGNAL( ruleAdded(   CSecureRule* ) ), this, SLOT( addRule(    CSecureRule* ) ), Qt::QueuedConnection );
	connect( &Security, SIGNAL( ruleRemoved( CSecureRule* ) ), this, SLOT( removeRule( CSecureRule* ) ), Qt::QueuedConnection );
}

CSecurityTableModel::~CSecurityTableModel()
{
	qDeleteAll( m_lRules );
	m_lRules.clear();
}

int CSecurityTableModel::rowCount(const QModelIndex& parent) const
{
	if ( parent.isValid() )
	{
		return 0;
	}
	else
	{
		return m_lRules.count();
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
	if ( !index.isValid() || index.row() > m_lRules.size() || index.row() < 0 )
	{
		return QVariant();
	}

	const Rule* pRule = m_lRules.at( index.row() );

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
	if ( parent.isValid() || row < 0 || row >= m_lRules.count() )
		return QModelIndex();
	else
		return createIndex( row, column, m_lRules[row] );
}

class CSecurityTableModelCmp
{
public:
	CSecurityTableModelCmp( int col, Qt::SortOrder o ) :
		column( col ),
		order( o )
	{}

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
	qStableSort( m_lRules.begin(), m_lRules.end(), CSecurityTableModelCmp( column, order ) );
	emit layoutChanged();
}

/*security::CSecureRule* CSecurityTableModel::nodeFromIndex(const QModelIndex &index)
{
	if ( index.isValid() && index.row() < m_lNodes.count() && index.row() >= 0 )
		return m_lRules[ index.row() ]->pNode;
	else
		return 0;
}*/

void CSecurityTableModel::addRule(CSecureRule* pRule)
{
	if ( Security.check( pRule ) )
	{
		beginInsertRows( QModelIndex(), m_lRules.size(), m_lRules.size() );
		m_lRules.append( new Rule( pRule ) );
		endInsertRows();
		m_bNeedSorting = true;
	}
}

void CSecurityTableModel::removeRule(CSecureRule* pRule)
{
	for ( quint32 i = 0, nMax = m_lRules.size(); i < nMax; i++ )
	{
		if ( m_lRules[i]->pRule == pRule )
		{
			beginRemoveRows( QModelIndex(), i, i );
			delete m_lRules[i];
			m_lRules.remove( i, 1 );
			endRemoveRows();
			m_bNeedSorting = true;
			break;
		}
	}

	delete pRule;
	pRule = NULL;
}

void CSecurityTableModel::updateAll()
{
	QModelIndexList uplist;
	bool bSort = m_bNeedSorting;

	if ( Security.m_pSection.tryLock() )
	{
		for ( quint32 i = 0, max = m_lRules.count(); i < max; ++i )
		{
			if ( m_lRules[i]->update( i, m_nSortColumn, uplist, this ) )
				bSort = true;
		}

		Security.m_pSection.unlock();
	}

	if( bSort )
	{
		sort( m_nSortColumn, m_nSortOrder );
	}
	else
	{
		if ( !uplist.isEmpty() )
		{
			foreach( QModelIndex index, uplist )
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
