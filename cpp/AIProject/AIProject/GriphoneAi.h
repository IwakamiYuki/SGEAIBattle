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

// なんかstatic constで定義すると怒られるorz
#define EPSILON 1.0e-9

class GriphoneAI : public AIInterface {
public:
    // 角度の上限
    static const int MAX_RANGE = 12;
    // 次の自分のターンまでの移動量
    static const int WALK_DIST_PER_TURN = 10;
    // 最大ターン数
    static const int MAX_TURN = 1800;
    // プレイヤー数
    static const int PLAYER_COUNT = 3;
    // 目的地とする距離
    static const int ALLOWANCE_DISTANCE = 10000;
    // 敵がこの数値以内なら攻撃を仕掛ける
    static const int ATTACK_THRESHOLD_TURN = 3;
    // プレイヤーの半径
    static const int PLAYER_RADIUS = 80;
    // スタン攻撃の割合
    static const int STUN_ATTACK_PERCENTAGE = 66;

    // getDiffAngle用
    // -180 ~ 180以内に収める
    static const int DIFF_ANGLE_WITHIN_180 = 1;
    // -12~ 12以内に収める
    static const int DIFF_ANGLE_WITHIN_12  = 2;

    // 近くのコインまでの距離の逆数にかける係数。ターン距離と比較に用いる
    static const int NEAR_COIN_INVERSE_LENGTH_FACTOR = 1000;

    GriphoneAI();
    virtual TimeLength GetTimeLength(float startX, float startY, int startAngle, float targetX, float targetY);
    virtual Command Update(TurnData turnData);
    virtual float getLengthSquare(float x1, float y1, float x2, float y2);
    virtual int adjustAngle(int angle);
    virtual int adjustRange(int angle);
    virtual int getDiffAngle(int currentAngle, int x1, int y1, int x2, int y2, int option);
    virtual bool canAttack(double sStartX, double sStartY, double angle, int coolTime, int stunTime, double Px, double Py);
    virtual double getDistanceLinePoint(double sStartX, double sStartY, double angle, int scalar, double Px, double Py);
};

#endif
