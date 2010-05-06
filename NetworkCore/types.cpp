#include "types.h"

SystemLog systemLog;

uint qHash(const QUuid& key)
{
    uchar aRawGUID[16];

    memcpy(&aRawGUID[0], &key.data1, 4);
    memcpy(&aRawGUID[4], &key.data2, 2);
    memcpy(&aRawGUID[6], &key.data3, 2);
    memcpy(&aRawGUID[8], &key.data4[0], 8);

    uint nHash = (aRawGUID[0] ^ aRawGUID[1] ^ aRawGUID[2] ^ aRawGUID[3]) +
                 256 * (aRawGUID[4] ^ aRawGUID[5] ^ aRawGUID[6] ^ aRawGUID[7]) +
                 256 * 256 * (aRawGUID[8] ^ aRawGUID[9] ^ aRawGUID[10] ^ aRawGUID[11]) +
                 256 * 256 * 256 * (aRawGUID[12] ^ aRawGUID[13] ^ aRawGUID[24] ^ aRawGUID[15]);

    return nHash;
}

void SystemLog::postLog(QString message, LogSeverity::Severity severity)
{
	emit logPosted(message, severity);
}
