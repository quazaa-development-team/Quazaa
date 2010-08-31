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

#include "FileHasher.h"
#include "Hashes/hash.h"
#include <QFile>
#include <QByteArray>
#include "ShareManager.h"
#include "quazaasettings.h"
#include <QTime>

QMutex CFileHasher::m_pSection;
QQueue<CSharedFilePtr> CFileHasher::m_lQueue;
CFileHasher** CFileHasher::m_pHashers = 0;
quint32  CFileHasher::m_nMaxHashers = 1;
quint32  CFileHasher::m_nRunningHashers = 0;
QWaitCondition CFileHasher::m_oWaitCond;

CFileHasher::CFileHasher(QObject* parent) :QThread(parent)
{
	m_bActive = true;
}
CFileHasher::~CFileHasher()
{
	m_bActive = false;
	if( isRunning() )
		wait();
}

CFileHasher* CFileHasher::HashFile(CSharedFilePtr pFile)
{
	m_pSection.lock();

	if( m_pHashers == 0 )
	{
		m_nMaxHashers = qMax<quint32>(2, QThread::idealThreadCount());
		m_pHashers = new CFileHasher*[m_nMaxHashers];
		for( int i = 0; i < m_nMaxHashers; i++ )
			m_pHashers[i] = 0;
	}

	//qDebug() << "File" << pFile->m_sFilename << "queued for hashing";
	m_lQueue.enqueue(pFile);

	CFileHasher* pHasher = 0;

	if( m_nRunningHashers < m_lQueue.size() && m_nRunningHashers < m_nMaxHashers )
	{
		for( int i = 0; i < m_nMaxHashers; i++ )
		{
			if( !m_pHashers[i] )
			{
				qDebug() << "Starting hasher: " << m_nRunningHashers;
				m_pHashers[i] = new CFileHasher();
				connect(m_pHashers[i], SIGNAL(QueueEmpty()), &ShareManager, SLOT(RunHashing()), Qt::UniqueConnection);
				connect(m_pHashers[i], SIGNAL(FileHashed(CSharedFilePtr)), &ShareManager, SLOT(OnFileHashed(CSharedFilePtr)), Qt::UniqueConnection);
				m_pHashers[i]->start((quazaaSettings.Library.HighPriorityHashing ? QThread::NormalPriority : QThread::LowestPriority));
				m_nRunningHashers++;
				pHasher = m_pHashers[i];
				break;
			}
		}
	}

	m_pSection.unlock();

	CFileHasher::m_oWaitCond.wakeOne();

	return pHasher;
}
void CFileHasher::run()
{
	QTime tTime;
	QByteArray baBuffer;

	const int nBufferSize = 64 * 1024;

	m_pSection.lock();

	while( !m_lQueue.isEmpty() )
	{
		CSharedFilePtr pFile = m_lQueue.dequeue();
		qDebug() << "Hashing" << pFile->m_sFileName;
		tTime.start();
		m_pSection.unlock();

		bool bHashed = true;

		QFile f;
		f.setFileName(pFile->m_sDirectory + QString("/") + pFile->m_sFileName);

		if( f.exists() && f.open(QFile::ReadOnly) )
		{
			baBuffer.resize(nBufferSize);

			pFile->m_lHashes.append(new CHash(CHash::SHA1));

			while( !f.atEnd() )
			{
				if( !m_bActive )
				{
					qDebug() << "CFileHasher aborting...";
					bHashed = false;
					break;
				}
				qint64 nRead = f.read(baBuffer.data(), nBufferSize);

				if( nRead < 0 )
				{
					bHashed = false;
					qDebug() << "File read error:" << f.error();
					break;
				}
				else if ( nRead < nBufferSize )
				{
					baBuffer.resize(nRead);
				}

				for( int i = 0; i < pFile->m_lHashes.size(); i++ )
				{
					pFile->m_lHashes[i]->AddData(baBuffer);
				}

			}

			f.close();
		}
		else
		{
			qDebug() << "File open error: " << f.error();
			bHashed = false;
		}

		if( bHashed )
		{
			double nRate = (f.size() / (tTime.elapsed() / 1000.0)) / 1024.0 / 1024.0;
			qDebug() << "File " << pFile->m_sFileName << "hashed at" << nRate << "MB/s:";
			for( int i = 0; i < pFile->m_lHashes.size(); i++ )
			{
				pFile->m_lHashes[i]->Finalize();
				qDebug() << pFile->m_lHashes[i]->ToURN();
			}
			emit FileHashed(pFile);
		}

		m_pSection.lock();

		if( !m_bActive )
			break;

		if( bHashed && m_lQueue.isEmpty() )
		{
			emit QueueEmpty();
			qDebug() << "Hasher " << this << "waiting...";
			CFileHasher::m_oWaitCond.wait(&m_pSection, 10000);
		}
	}

	for( int i = 0; i < m_nMaxHashers; i++ )
	{
		if( m_pHashers[i] == this )
		{
			m_pHashers[i] = 0;
			deleteLater();
			m_nRunningHashers--;

			if( m_nRunningHashers == 0 )
			{
				delete [] m_pHashers;
				m_pHashers = 0;
			}
			break;
		}
	}

	m_pSection.unlock();

	qDebug() << "CFileHasher done. " << m_nRunningHashers;
}

