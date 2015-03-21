#ifndef AIProject_GriphoneAi_h
#define AIProject_GriphoneAi_h
#define _USE_MATH_DEFINES
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <stdlib.h>
#include <cmath>

#include "Types.h"
#include "AIInterface.h"

class GriphoneAI : public AIInterface {
public:
    GriphoneAI();
    virtual TimeLength GetTimeLength(float startX, float startY, int startAngle, float targetX, float targetY);
    virtual Command Update(TurnData turnData);
    virtual float getLengthSquare(float x1, float y1, float x2, float y2);
};

#endif