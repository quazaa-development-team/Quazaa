#ifndef CHATCONVERTER_H
#define CHATCONVERTER_H

class QTextDocument;
class QTextBlock;
class QTextFragment;

#include <QString>
#include <QStack>
#include <QColor>

class CChatConverter
{

	enum OutputFormats
	{
		HTML,
		BBCODE,
		IRCCODE
	};

	QString			m_pResult;
	QTextDocument*	m_pDocument;

	QStack<QString> m_pStack;

	OutputFormats	m_nOutputFormat;

public:
	CChatConverter(QTextDocument* pDoc);
	virtual ~CChatConverter();

	QString toHtml();
	QString toBBCode();
	QString toIRC();
private:
	void processBlock(QTextBlock* pBlock);
	void processFragment(QTextFragment* pFrag);
	void processFragmentBB(QTextFragment* pFrag);
	void processFragmentIRC(QTextFragment* pFrag);
};

#endif // CHATCONVERTER_H
