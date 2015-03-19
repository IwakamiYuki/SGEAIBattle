#ifndef __AIBattle__Sample__
#define __AIBattle__Sample__

#include "Types.h"
#include "AIInterface.h"

class SampleAI : public AIInterface {
public:
    SampleAI();
    virtual Command Update(TurnData turnData);
};

#endif