#include "chatconverter.h"

#include <QTextDocument>
#include <QTextBlock>
#include <QTextFragment>
#include <QTextCharFormat>

#include <QDebug>

CChatConverter::CChatConverter(QTextDocument *pDoc) :
		m_pDocument(pDoc)
{
}
CChatConverter::~CChatConverter()
{

}
QString CChatConverter::toHtml()
{
	m_nOutputFormat = HTML;
	m_pResult.clear();
	m_pStack.clear();

	QTextBlock currentBlock = m_pDocument->begin();

	while (currentBlock.isValid()) {
		processBlock(&currentBlock);
		currentBlock = currentBlock.next();
	}

	return m_pResult;
}
QString CChatConverter::toBBCode()
{
	m_nOutputFormat = BBCODE;
	m_pResult.clear();
	m_pStack.clear();

	QTextBlock currentBlock = m_pDocument->begin();

	while (currentBlock.isValid()) {
		processBlock(&currentBlock);
		currentBlock = currentBlock.next();
	}

	return m_pResult;
}

void CChatConverter::processBlock(QTextBlock *pBlock)
{
	QTextBlock::iterator it;
	for (it = pBlock->begin(); !(it.atEnd()); ++it) {
		QTextFragment currentFragment = it.fragment();
		if (currentFragment.isValid())
		{
			switch( m_nOutputFormat )
			{
			case HTML:
				processFragment(&currentFragment);
				break;
			case BBCODE:
				processFragmentBB(&currentFragment);
				break;
			}
		}
	}

	while( !m_pStack.isEmpty() )
	{
		QString sTag = m_pStack.pop();
		switch( m_nOutputFormat )
		{
		case HTML:
			m_pResult.append("</" + sTag + ">");
			break;
		case BBCODE:
			m_pResult.append("[/" + sTag + "]");
			break;
		}
	}
}
void CChatConverter::processFragment(QTextFragment *pFrag)
{
	QTextCharFormat fmt = pFrag->charFormat();

	bool bLoop = true;
	while( bLoop && !m_pStack.isEmpty() )
	{
		bLoop = false;

		if( m_pStack.top() == "b" && fmt.fontWeight() <= QFont::Normal )
		{
			m_pResult.append("</b>");
			bLoop = true;
			m_pStack.pop();
		}
		else if( m_pStack.top() == "i" && !fmt.fontItalic() )
		{
			m_pResult.append("</i>");
			bLoop = true;
			m_pStack.pop();
		}
		else if( m_pStack.top() == "u" && !fmt.fontUnderline() )
		{
			m_pResult.append("</u>");
			bLoop = true;
			m_pStack.pop();
		}

	}

	if( fmt.fontWeight() > QFont::Normal && !m_pStack.contains("b") )
	{
		m_pResult.append("<b>");
		m_pStack.push("b");
	}

	if( fmt.fontItalic() && !m_pStack.contains("i"))
	{
		m_pResult.append("<i>");
		m_pStack.push("i");
	}

	if( fmt.fontUnderline() && !m_pStack.contains("u"))
	{
		m_pResult.append("<u>");
		m_pStack.push("u");
	}

	m_pResult.append(pFrag->text());
}

void CChatConverter::processFragmentBB(QTextFragment *pFrag)
{
	QTextCharFormat fmt = pFrag->charFormat();

	bool bLoop = true;
	while( bLoop && !m_pStack.isEmpty() )
	{
		bLoop = false;

		if( m_pStack.top() == "b" && fmt.fontWeight() <= QFont::Normal )
		{
			m_pResult.append("[/b]");
			bLoop = true;
			m_pStack.pop();
		}
		else if( m_pStack.top() == "i" && !fmt.fontItalic() )
		{
			m_pResult.append("[/i]");
			bLoop = true;
			m_pStack.pop();
		}
		else if( m_pStack.top() == "u" && !fmt.fontUnderline() )
		{
			m_pResult.append("[/u]");
			bLoop = true;
			m_pStack.pop();
		}

	}

	if( fmt.fontWeight() > QFont::Normal && !m_pStack.contains("b") )
	{
		m_pResult.append("[b]");
		m_pStack.push("b");
	}

	if( fmt.fontItalic() && !m_pStack.contains("i"))
	{
		m_pResult.append("[i]");
		m_pStack.push("i");
	}

	if( fmt.fontUnderline() && !m_pStack.contains("u"))
	{
		m_pResult.append("[u]");
		m_pStack.push("u");
	}

	m_pResult.append(pFrag->text());
}
