#include "securityfiltermodel.h"

SecurityFilterModel::SecurityFilterModel(CSecurityTableModel *target, bool bShowAutomatic)
{
	m_lSecurity = target;
	setSourceModel(m_lSecurity);
	m_bShowAutomatic = bShowAutomatic;
	invalidateFilter();
}

void SecurityFilterModel::sort(int column, Qt::SortOrder order)
{
	m_lSecurity->sort(column, order);
}

bool SecurityFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
	CSecureRule* pRule = m_lSecurity->ruleFromIndex( m_lSecurity->index(sourceRow, 0, sourceParent) );

	if(m_bShowAutomatic) {
		return pRule->m_bAutomatic;
	}

	return !pRule->m_bAutomatic;
}
