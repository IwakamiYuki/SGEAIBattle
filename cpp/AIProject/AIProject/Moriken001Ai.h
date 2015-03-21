#ifndef AIProject_Moriken001Ai_h
#define AIProject_Moriken001Ai_h
#define _USE_MATH_DEFINES
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <stdlib.h>
#include <cmath>

#include "Types.h"
#include "AIInterface.h"

class Moriken001AI : public AIInterface {
public:
    Moriken001AI();
    virtual TimeLength GetTimeLength(float startX, float startY, int startAngle, float targetX, float targetY);
    virtual Command Update(TurnData turnData);
};

#endif
