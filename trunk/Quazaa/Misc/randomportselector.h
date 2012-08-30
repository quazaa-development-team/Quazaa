#ifndef RANDOMPORTSELECTOR_H
#define RANDOMPORTSELECTOR_H

#include <set>

namespace common
{
static std::set<short> oUsedPorts;
static short getRandomUnusedPort();

}

#endif // RANDOMPORTSELECTOR_H
