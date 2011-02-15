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
QString CChatConverter::toIRC()
{
	m_nOutputFormat = IRCCODE;
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
			case IRCCODE:
				processFragmentIRC(&currentFragment);
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
		case IRCCODE:
			m_pResult.append(sTag);
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

void CChatConverter::processFragmentIRC(QTextFragment *pFrag)
{
	QTextCharFormat fmt = pFrag->charFormat();

	bool bLoop = true;
	while( bLoop && !m_pStack.isEmpty() )
	{
		bLoop = false;

		if( m_pStack.top() == "\x02" && fmt.fontWeight() <= QFont::Normal )
		{
			m_pResult.append("\x02");
			bLoop = true;
			m_pStack.pop();
		}
		else if( m_pStack.top() == "\x09" && !fmt.fontItalic() )
		{
			m_pResult.append("\x09");
			bLoop = true;
			m_pStack.pop();
		}
		else if( m_pStack.top() == "\x15" && !fmt.fontUnderline() )
		{
			m_pResult.append("\x15");
			bLoop = true;
			m_pStack.pop();
		}
		else if( m_pStack.top() == "0" && (fmt.foreground().color() != QColor("white")) )
		{
			m_pResult.append("\017");
			bLoop = true;
			m_pStack.pop();
		}
		else if( m_pStack.top() == "2" && (fmt.foreground().color() != QColor("navy")) )
		{
			m_pResult.append("\017");
			bLoop = true;
			m_pStack.pop();
		}
		else if( m_pStack.top() == "3" && (fmt.foreground().color() != QColor("green")) )
		{
			m_pResult.append("\017");
			bLoop = true;
			m_pStack.pop();
		}
		else if( m_pStack.top() == "4" && (fmt.foreground().color() != QColor("red")) )
		{
			m_pResult.append("\017");
			bLoop = true;
			m_pStack.pop();
		}
		else if( m_pStack.top() == "5" && (fmt.foreground().color() != QColor("brown")) )
		{
			m_pResult.append("\017");
			bLoop = true;
			m_pStack.pop();
		}
		else if( m_pStack.top() == "6" && (fmt.foreground().color() != QColor("purple")) )
		{
			m_pResult.append("\017");
			bLoop = true;
			m_pStack.pop();
		}
		else if( m_pStack.top() == "7" && (fmt.foreground().color() != QColor("olive")) )
		{
			m_pResult.append("\017");
			bLoop = true;
			m_pStack.pop();
		}
		else if( m_pStack.top() == "8" && (fmt.foreground().color() != QColor("yellow")) )
		{
			m_pResult.append("\017");
			bLoop = true;
			m_pStack.pop();
		}
		else if( m_pStack.top() == "9" && (fmt.foreground().color() != QColor("lime")) )
		{
			m_pResult.append("\017");
			bLoop = true;
			m_pStack.pop();
		}
		else if( m_pStack.top() == "10" && (fmt.foreground().color() != QColor("darkcyan")) )
		{
			m_pResult.append("\017");
			bLoop = true;
			m_pStack.pop();
		}
		else if( m_pStack.top() == "11" && (fmt.foreground().color() != QColor("cyan")) )
		{
			m_pResult.append("\017");
			bLoop = true;
			m_pStack.pop();
		}
		else if( m_pStack.top() == "12" && (fmt.foreground().color() != QColor("blue")) )
		{
			m_pResult.append("\017");
			bLoop = true;
			m_pStack.pop();
		}
		else if( m_pStack.top() == "13" && (fmt.foreground().color() != QColor("magenta")) )
		{
			m_pResult.append("\017");
			bLoop = true;
			m_pStack.pop();
		}
		else if( m_pStack.top() == "14" && (fmt.foreground().color() != QColor("gray")) )
		{
			m_pResult.append("\017");
			bLoop = true;
			m_pStack.pop();
		}
		else if( m_pStack.top() == "15" && (fmt.foreground().color() != QColor("lightgray")) )
		{
			m_pResult.append("\017");
			bLoop = true;
			m_pStack.pop();
		}

	}

	if( fmt.fontWeight() > QFont::Normal && !m_pStack.contains("\x02") )
	{
		m_pResult.append("\x02");
		m_pStack.push("\x02");
	}

	if( fmt.fontItalic() && !m_pStack.contains("\x09"))
	{
		m_pResult.append("\x09");
		m_pStack.push("\x09");
	}

	if( fmt.fontUnderline() && !m_pStack.contains("\x15"))
	{
		m_pResult.append("\x15");
		m_pStack.push("\x15");
	}
	else if( (fmt.foreground().color() == QColor("white")) && !m_pStack.contains("0"))
	{
		m_pResult.append("0");
		m_pStack.push("0");
	}
	else if( (fmt.foreground().color() == QColor("black")) && !m_pStack.contains("\017"))
	{
		m_pResult.append("\017");
		m_pStack.push("\017");
	}
	else if( (fmt.foreground().color() == QColor("navy")) && !m_pStack.contains("2"))
	{
		m_pResult.append("2");
		m_pStack.push("2");
	}
	else if( (fmt.foreground().color() == QColor("green")) && !m_pStack.contains("3"))
	{
		m_pResult.append("3");
		m_pStack.push("3");
	}
	else if( (fmt.foreground().color() == QColor("red")) && !m_pStack.contains("4"))
	{
		m_pResult.append("4");
		m_pStack.push("4");
	}
	else if( (fmt.foreground().color() == QColor("brown")) && !m_pStack.contains("5"))
	{
		m_pResult.append("5");
		m_pStack.push("5");
	}
	else if( (fmt.foreground().color() == QColor("purple")) && !m_pStack.contains("6"))
	{
		m_pResult.append("6");
		m_pStack.push("6");
	}
	else if( (fmt.foreground().color() == QColor("olive")) && !m_pStack.contains("7"))
	{
		m_pResult.append("7");
		m_pStack.push("7");
	}
	else if( (fmt.foreground().color() == QColor("yellow")) && !m_pStack.contains("8"))
	{
		m_pResult.append("8");
		m_pStack.push("8");
	}
	else if( (fmt.foreground().color() == QColor("lime")) && !m_pStack.contains("9"))
	{
		m_pResult.append("9");
		m_pStack.push("9");
	}
	else if( (fmt.foreground().color() == QColor("darkcyan")) && !m_pStack.contains("10"))
	{
		m_pResult.append("10");
		m_pStack.push("10");
	}
	else if( (fmt.foreground().color() == QColor("cyan")) && !m_pStack.contains("11"))
	{
		m_pResult.append("11");
		m_pStack.push("11");
	}
	else if( (fmt.foreground().color() == QColor("blue")) && !m_pStack.contains("12"))
	{
		m_pResult.append("12");
		m_pStack.push("12");
	}
	else if( (fmt.foreground().color() == QColor("magenta")) && !m_pStack.contains("13"))
	{
		m_pResult.append("13");
		m_pStack.push("13");
	}
	else if( (fmt.foreground().color() == QColor("gray")) && !m_pStack.contains("14"))
	{
		m_pResult.append("14");
		m_pStack.push("14");
	}
	else if( (fmt.foreground().color() == QColor("lightgray")) && !m_pStack.contains("15"))
	{
		m_pResult.append("15");
		m_pStack.push("15");
	}

	m_pResult.append(pFrag->text());
}
