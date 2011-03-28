//
// thread.h
//
// Copyright © Quazaa Development Team, 2009-2010.
// This file is part of QUAZAA (quazaa.sourceforge.net)
//
// Quazaa is free software; you can redistribute it
// and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3 of
// the License, or (at your option) any later version.
//
// Quazaa is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Quazaa; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#ifndef THREAD_H
#define THREAD_H

#include "types.h"

#include <QThread>
#include <QMutex>
#include <QMutexLocker>
#include <QWaitCondition>

class CThread: public QThread
{
	Q_OBJECT

	QMutex*         m_pMutex;
	QWaitCondition  m_oStartCond;
	QObject*        m_pTargetObject;
	QString			m_sThreadName;
public:
	CThread(QObject* parent = 0);

	void start(QString strName, QMutex* pMutex, QObject* pTargetObj = 0, Priority p = InheritPriority)
	{
		m_sThreadName = strName;

		qDebug() << strName << "Thread::start";
		//QMutexLocker l(pMutex);
		m_pMutex = pMutex;
		m_pTargetObject = pTargetObj;
		if(pTargetObj)
		{
			pTargetObj->moveToThread(this);
		}
		qDebug() << strName << "Starting...";
		QThread::start(p);
		qDebug() << strName << "Waiting for thread to start...";
		if(!isRunning())
		{
			m_oStartCond.wait(m_pMutex);
		}
		qDebug() << strName << "Thread started";
	}

	void exit(int retcode)
	{
		//QMutexLocker l(m_pMutex);
		qDebug() << m_sThreadName << "Exiting thread";
		QThread::exit(retcode);
		qDebug() << m_sThreadName << "Waiting for thread to finish...";

		if(isRunning())
		{
			m_oStartCond.wait(m_pMutex);
		}
		//wait();
		qDebug() << m_sThreadName << "Thread Finished";
	}

protected:
	void run()
	{
		QMutexLocker l(m_pMutex);
		msleep(50);

		if(m_pTargetObject)
		{
			if(!QMetaObject::invokeMethod(m_pTargetObject, "SetupThread", Qt::DirectConnection))
			{
				qWarning() << "Failed to call target's SetupThread method";
			}
		}

		m_oStartCond.wakeAll();

		l.unlock();

		exec();

		l.relock();

		if(m_pTargetObject)
		{
			if(!QMetaObject::invokeMethod(m_pTargetObject, "CleanupThread", Qt::DirectConnection))
			{
				qWarning() << "Failed to call target's CleanupThread method";
			}
		}

		msleep(50);
		m_oStartCond.wakeAll();
	}
};


#endif // THREAD_H
