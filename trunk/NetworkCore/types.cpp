#include "types.h"
#include <QMetaType>

uint qHash(const QUuid& key)
{
	uint nHash = 0;
	nHash ^= key.data1;
	nHash ^= key.data2;
	nHash ^= (key.data3 << 16);
	/*
	The void* cast below is correct,
	this is to avoid warnings like:
	"dereferencing type-punned pointer will break strict-aliasing rules"
	when compiling with -fstrict-aliasing (enabled by default when using -O2)
	*/
	nHash ^= *(quint32*)(void*)&key.data4[0];
	nHash ^= *(quint32*)(void*)&key.data4[4];

    return nHash;
}
