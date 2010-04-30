#ifndef THREAD_H
#define THREAD_H

#include <QThread>
#include <QMutex>
#include <QMutexLocker>
#include <QWaitCondition>

class CThread:public QThread
{
    Q_OBJECT

    QMutex*         m_pMutex;
    QWaitCondition  m_oStartCond;
    QObject*        m_pTargetObject;
public:
    CThread(QObject* parent = 0);

    void start(QMutex* pMutex, QObject* pTargetObj = 0, Priority p = InheritPriority)
    {
        qDebug() << "Thread::start";
        //QMutexLocker l(pMutex);
        m_pMutex = pMutex;
        m_pTargetObject = pTargetObj;
        if( pTargetObj )
            pTargetObj->moveToThread(this);
        qDebug() << "Starting...";
        QThread::start(p);
        qDebug() << "Waiting for thread to start...";
        if( !isRunning() )
            m_oStartCond.wait(m_pMutex);
        qDebug() << "Thread started";
    }

    void exit(int retcode)
    {
        //QMutexLocker l(m_pMutex);
        qDebug() << "Exiting thread";
        QThread::exit(retcode);
        qDebug() << "Waiting for thread to finish...";

        if( isRunning() )
            m_oStartCond.wait(m_pMutex);
        //wait();
        qDebug() << "Thread Finished";
    }

protected:
    void run()
    {
        QMutexLocker l(m_pMutex);
        msleep(50);

        if( m_pTargetObject)
        {
            if( !QMetaObject::invokeMethod(m_pTargetObject, "SetupThread", Qt::DirectConnection) )
                qWarning() << "Failed to call target's SetupThread method";
        }

        m_oStartCond.wakeAll();

        l.unlock();

        exec();

        l.relock();

        if( m_pTargetObject)
        {
            if( !QMetaObject::invokeMethod(m_pTargetObject, "CleanupThread", Qt::DirectConnection) )
                qWarning() << "Failed to call target's CleanupThread method";
        }

        msleep(50);
        m_oStartCond.wakeAll();
    }
};


#endif // THREAD_H
