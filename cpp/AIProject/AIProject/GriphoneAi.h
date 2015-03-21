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
    // 角度の上限
    const int MAX_RANGE = 12;
    // 次の自分のターンまでの移動量
    const int MOVEMENT = 30;
    // 最大ターン数
    const int MAX_TURN = 1800;
    // プレイヤー数
    const int PLAYER_COUNT = 3;
    GriphoneAI();
    virtual TimeLength GetTimeLength(float startX, float startY, int startAngle, float targetX, float targetY);
    virtual Command Update(TurnData turnData);
    virtual float getLengthSquare(float x1, float y1, float x2, float y2);
};

#endif
