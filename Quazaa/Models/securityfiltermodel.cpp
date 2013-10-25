#include "securityfiltermodel.h"

SecurityFilterModel::SecurityFilterModel(CSecurityTableModel *target, bool bShowAutomatic)
{
	m_lSecurity = target;
	setSourceModel(m_lSecurity);
	m_bShowAutomatic = bShowAutomatic;
	invalidateFilter();
}

bool SecurityFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
	CSecureRule* pRule = m_lSecurity->ruleFromIndex( m_lSecurity->index(sourceRow, 0, sourceParent) );

	if(m_bShowAutomatic) {
		return pRule->m_bAutomatic;
	}

	return !pRule->m_bAutomatic;
}
