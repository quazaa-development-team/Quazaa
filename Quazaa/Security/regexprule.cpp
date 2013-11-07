#include "regexprule.h"

CRegularExpressionRule::CRegularExpressionRule()
{
	m_nType = RuleType::RegularExpression;
	m_bSpecialElements = false;
}

CSecureRule* CRegularExpressionRule::getCopy() const
{
	return new CRegularExpressionRule( *this );
}

bool CRegularExpressionRule::operator==(const CSecureRule& pRule) const
{
	return CSecureRule::operator==( pRule ) &&
		   m_bSpecialElements == ((CRegularExpressionRule*)&pRule)->m_bSpecialElements;
}

bool CRegularExpressionRule::parseContent(const QString& sContent)
{
	m_sContent = sContent.trimmed();

	quint8 nCount = 0;
	for ( quint8 i = 1; i < 10; i++ )
	{
		if ( m_sContent.contains( "<" + QString::number( i ) + ">" ) )
		{
			nCount++;
		}
	}

	if ( nCount || m_sContent.contains( "<_>" ) || m_sContent.contains( "<>" ) )
	{
		// In this case the regular expression must be build ech time a filter request comes in,
		// so theres no point in doing it here.
		m_bSpecialElements = true;
		return true;
	}
	else
	{
		m_bSpecialElements = false;
		bool bValid;

		m_regularExpressionContent = QRegularExpression( m_sContent );
		bValid = m_regularExpressionContent.isValid();

		return bValid;
	}
}

bool CRegularExpressionRule::match(const QList<QString>& lQuery, const QString& sContent) const
{
	Q_ASSERT( m_nType == RuleType::RegularExpression );

	if ( m_sContent.isEmpty() )
		return false;

	if ( m_bSpecialElements )
	{
		// Build a regular expression filter from the search query words.
		// Returns an empty string if not applied or if the filter was invalid.
		//
		// Substitutes:
		// <_> - inserts all query keywords;
		// <0>..<9> - inserts query keyword number 0..9;
		// <> - inserts next query keyword.
		//
		// For example regular expression:
		//	.*(<2><1>)|(<_>).*
		// for "music mp3" query will be converted to:
		//	.*(mp3\s*music\s*)|(music\s*mp3\s*).*
		//
		// Note: \s* - matches any number of white-space symbols (including zero).

		QString sFilter, sBaseFilter = m_sContent;

		int pos = sBaseFilter.indexOf( '<' );
		if ( pos != -1 )
		{
			quint8 nArg = 0;

			// replace all relevant occurrences of <*something*
			while ( pos != -1 );
			{
				sFilter += sBaseFilter.left( pos );
				sBaseFilter.remove( 0, pos );
				bool bSuccess = replace( sBaseFilter, lQuery, nArg );

				pos = sBaseFilter.indexOf( '<', bSuccess ? 0 : 1 );
			}
			// add whats left of the base filter string to the newly generated filter
			sFilter += sBaseFilter;

			QRegularExpression oRegExpFilter = QRegularExpression( sFilter );
			return oRegExpFilter.match( sContent ).hasMatch();
		}
		else
		{
			// This shouldn't happen, but it's covered anyway...
			Q_ASSERT( false );

			QRegularExpression oRegExpFilter = QRegularExpression( m_sContent );
			return oRegExpFilter.match( sContent ).hasMatch();
		}
	}
	else
	{
		return m_regularExpressionContent.match( sContent ).hasMatch();
	}
}

void CRegularExpressionRule::toXML(QXmlStreamWriter& oXMLdocument) const
{
	Q_ASSERT( m_nType == RuleType::RegularExpression );

	oXMLdocument.writeStartElement( "rule" );

	oXMLdocument.writeAttribute( "type", "regexp" );
	oXMLdocument.writeAttribute( "content", getContentString() );

	CSecureRule::toXML( *this, oXMLdocument );

	oXMLdocument.writeEndElement();
}

bool CRegularExpressionRule::replace(QString& sReplace, const QList<QString>& lQuery, quint8& nCurrent)
{
	if ( sReplace.at( 0 ) != '<' )
		return false;

	if ( sReplace.length() > 1 && sReplace.at( 1 ) == '>' )
	{
		sReplace.remove( 0, 2 );
		if ( nCurrent < lQuery.size() )
		{
			sReplace = lQuery.at( nCurrent ) + "\\s*" + sReplace;
		}
		nCurrent++;
		return true;
	}

	if ( sReplace.length() > 2 && sReplace.at( 2 ) == '>' )
	{
		if ( sReplace.at( 1 ) == '_' )
		{
			QString sMess;
			for ( quint8 i = 0; i < lQuery.size(); i++ )
			{
				sMess += lQuery.at( i );
				sMess += "\\s*";
			}
			sReplace.remove( 0, 3 );
			sReplace = sMess + sReplace;
			return true;
		}
		else
		{
			bool bSuccess;
			quint8 nArg = sReplace.mid( 1, 1 ).toShort( &bSuccess );
			if ( bSuccess )
			{
				sReplace.remove( 0, 3 );
				if ( nArg < lQuery.size() )
				{
					sReplace = lQuery.at( nArg ) + "\\s*" + sReplace;
				}
				return true;
			}
		}
	}
	return false;
}
