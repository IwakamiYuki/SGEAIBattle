#ifndef __AIBattle__AIInterface__
#define __AIBattle__AIInterface__

#include "Types.h"

class AIInterface {
public:
	FILE *logFp;
	FILE *logLocal;
    virtual Command Update(TurnData turnData) = 0;
};

#endif
