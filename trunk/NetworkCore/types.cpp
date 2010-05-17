#include "types.h"
#include <QMetaType>

uint qHash(const QUuid& key)
{
	uint nHash = 0;
	nHash ^= key.data1;
	nHash ^= key.data2;
	nHash ^= (key.data3 << 16);
	nHash ^= *(quint32*)&key.data4[0];
	nHash ^= *(quint32*)&key.data4[4];

    return nHash;
}
