#include <bitset>

const int MAX_COMPONENTS = 200;
const int MAX_ENTITIES = 200;

typedef std::bitset<MAX_COMPONENTS> ComponentMask;

typedef unsigned long long EntityID;
typedef unsigned int EntityIndex;
typedef unsigned int EntityVersion;

extern int s_componentCounter;
