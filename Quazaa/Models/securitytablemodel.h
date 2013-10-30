#ifndef SECURITYTABLEMODEL_H
#define SECURITYTABLEMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <QIcon>

#include "securitymanager.h"

class CSecurityTableModel : public QAbstractTableModel
{
	Q_OBJECT

private:
	QWidget*		m_oContainer;
	int				m_nSortColumn;
	Qt::SortOrder	m_nSortOrder;
	bool			m_bNeedSorting;

public:
	enum Column
	{
		CONTENT = 0,
		TYPE,
		ACTION,
		EXPIRES,
		HITS,
		COMMENT,
		_NO_OF_COLUMNS
	};

	// icons used for the different rules
	const QIcon* m_pIcons[3];

	struct Rule
	{
		// Object directly managed by security manager.
		CSecureRule*			m_pRule;

		QString					m_sContent;
		int						m_nType;
		RuleAction::Action		m_nAction;
		quint32					m_tExpire; // UTC time in seconds
		bool					m_bForever;
		quint32					m_nToday;
		quint32					m_nTotal;
		QString					m_sComment;
		const QIcon*			m_piAction;
		bool					m_bAutomatic;

		Rule(CSecureRule* pRule, CSecurityTableModel* model);
		~Rule();
		bool update(int row, int col, QModelIndexList& to_update, CSecurityTableModel* model);
		QVariant data(int col) const;
		bool lessThan(int col, const CSecurityTableModel::Rule* const pOther) const;

		QString actionToString(RuleAction::Action m_nAction) const;
		QString expiryToString(quint32 m_tExpire) const;
	};

protected:
	QList<Rule*>	m_lNodes;

public:
	explicit CSecurityTableModel(QObject* parent = 0, QWidget* container = 0);
	~CSecurityTableModel();

	int rowCount(const QModelIndex& parent = QModelIndex()) const;
	int columnCount(const QModelIndex& parent) const;
	QVariant data(const QModelIndex& index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;

	void sort(int column, Qt::SortOrder order);

	CSecureRule* ruleFromIndex(const QModelIndex& index);

	void completeRefresh();

public slots:
	void addRule(CSecureRule* pRule);
	void removeRule(CSecureRule *pRule);
	void updateAll();
};

#endif // SECURITYTABLEMODEL_H

