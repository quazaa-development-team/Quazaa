#ifndef CHATCONVERTER_H
#define CHATCONVERTER_H

class QTextDocument;
class QTextBlock;
class QTextFragment;

#include <QString>
#include <QStack>

class CChatConverter
{

	enum OutputFormats
	{
		HTML,
		BBCODE
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
private:
	void processBlock(QTextBlock* pBlock);
	void processFragment(QTextFragment* pFrag);
	void processFragmentBB(QTextFragment* pFrag);
};

#endif // CHATCONVERTER_H
