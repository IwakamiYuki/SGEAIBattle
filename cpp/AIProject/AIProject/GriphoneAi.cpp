
#include "GriphoneAI.h"
#include "Geometry.h"

using namespace std;

GriphoneAI::GriphoneAI()
{
	srand((unsigned) time(NULL));
}

/**
 * 移動してみて何ターンかかるかを推測する
 */
TimeLength GriphoneAI::GetTimeLength(float startX, float startY, int startAngle, float targetX, float targetY)
{
	TimeLength* timeLength = new TimeLength();

	float currentX = startX;
	float currentY = startY;
	int currentAngle = startAngle;


	// 初期値は大きめの値にする
	timeLength->turn = 6000;

	for (int i = 1; i < MAX_TURN / PLAYER_COUNT; i++)
	{
		// 現在の角度とターゲットまでの角度の差分
		// -180 ~ 180 に調整
		int ddAngle = getDiffAngle(currentAngle, currentX, currentY, targetX, targetY, DIFF_ANGLE_WITHIN_180);

		// ±12度以内に収める
		int dAngle = adjustRange(ddAngle);

		// 移動する
		currentAngle += dAngle;
		currentX += WALK_DIST_PER_TURN * PLAYER_COUNT * cos(currentAngle * M_PI / 180);
		currentY += WALK_DIST_PER_TURN * PLAYER_COUNT * sin(currentAngle * M_PI / 180);

		// ある程度まで近づいたら目的地に到着とする
		if (getLengthSquare(currentX, currentY, targetX, targetY) < ALLOWANCE_DISTANCE)
		{
			fprintf(logFp, "cul %d:	(%f,%f, %d)=>(%f,%f)	%d \n", i, currentX, currentY, currentAngle, targetX, targetY, ddAngle);
			timeLength->turn = i;
			timeLength->angle = currentAngle;
			break;
		}
	}
	return *timeLength;
}

/*
 * 距離を求める
 **/
float GriphoneAI::getLengthSquare(float x1, float y1, float x2, float y2)
{
	return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
}

/**
 * 与えられた角度を-180 ~ 180 に調整
 */
int GriphoneAI::adjustAngle(int angle)
{
	return (angle + 720 + 180) % 360 - 180;

}

/**
 * 2つのオブジェクトの位置から角度の差分を求める
 * currentAngleはオブジェクト1の向いている向き
 */
int GriphoneAI::getDiffAngle(int currentAngle, int x1, int y1, int x2, int y2, int option = 0)
{
		int dAngle = 180 / M_PI * atan2f(y2 - y1, x2 - x1) - currentAngle;
		if (option == DIFF_ANGLE_WITHIN_180 || option == DIFF_ANGLE_WITHIN_12) {
		  // -180 ~ 180 に調整
		  dAngle = adjustAngle(dAngle);
		  if (option == DIFF_ANGLE_WITHIN_12) {
		    // ±12度以内に収める
		    dAngle = adjustRange(dAngle);
		  }
		}
		return dAngle;
}

/**
 * 与えられた角度を-12 ~ 12に調整
 */
int GriphoneAI::adjustRange(int angle)
{
	// ±12度以内に収める
	if (angle > MAX_RANGE) angle = MAX_RANGE;
	if (angle < -MAX_RANGE) angle = -MAX_RANGE;
	return angle;

}

bool GriphoneAI::canAttack(double sStartX, double sStartY, double angle, double Px, double Py)
{
  double distance = getDistanceLinePoint(sStartX, sStartY, angle, PLAYER_RADIUS * 2, Px, Py);
  return distance + EPSILON <= 2 * PLAYER_RADIUS;
}

//線分と点の距離
double GriphoneAI::getDistanceLinePoint(double sStartX, double sStartY, double angle, int scalar, double Px, double Py) {
  pt point = pt(Px, Py);
  double radian = angle * M_PI / 180;
  // 終点の座標を求める
  pt endPoint = pt(scalar * cos(radian), scalar * sin(radian));
  line s = line(pt(sStartX, sStartY), endPoint);
  return distanceSP(s, point);
}


Command GriphoneAI::Update(TurnData turnData)
{
	Command* command = new Command();

	fprintf(logFp, "turn:%d********************\n", turnData.turn);
	fprintf(logFp, "myId:%d\n", turnData.myId);
	for (int i = 0; i < PLAYER_COUNT; i++)
	{
		fprintf(logFp, "player%d - - - - - - - - - - \n", i);
		PlayerData *pCurrentPlayerData = &turnData.playerList[i];
		fprintf(logFp, "pos.x:		%f\n", pCurrentPlayerData->pos.x);
		fprintf(logFp, "pos.y:		%f\n", pCurrentPlayerData->pos.y);
		fprintf(logFp, "angle:		%d\n", pCurrentPlayerData->angle);
		fprintf(logFp, "coin:		 %d\n", pCurrentPlayerData->coin);
		fprintf(logFp, "state:		%d\n", pCurrentPlayerData->state);
		fprintf(logFp, "coolTime:	 %d\n", pCurrentPlayerData->coolTime);
		fprintf(logFp, "stunTime:	 %d\n", pCurrentPlayerData->stunTime);
		fprintf(logFp, "lastAttackId: %d\n", pCurrentPlayerData->lastAttackId);
	}
	fprintf(logFp, "coinCount:%d\n", turnData.coinCount);
	for (int i = 0; i < turnData.coinCount; i++)
	{
		fprintf(logFp, "coin%d - - - - - - - - - - \n", i);
		CoinData *pCurrentCoinData = &turnData.coinList[i];
		fprintf(logFp, "pos.x:		%f\n", pCurrentCoinData->pos.x);
		fprintf(logFp, "pos.y:		%f\n", pCurrentCoinData->pos.y);
		fprintf(logFp, "appearTime:		%d\n", pCurrentCoinData->appearTime);
	}



	/* 初期設定 */
	// 自分自身
	PlayerData *pCurrentMyPlayerData = &turnData.playerList[turnData.myId];

	// 考えた結果を格納する変数
	int angle = 0;
	// 目的地
	float targetX = -1;
	float targetY = -1;

	// 行動コマンド。初期値は「移動」
	command->action = GameAction::Move;

	// 敵
	PlayerData *pEnemyPlayerData[] = {
	  &turnData.playerList[(turnData.myId + 1) % PLAYER_COUNT],
	  &turnData.playerList[(turnData.myId + 2) % PLAYER_COUNT]
	};

  for (int i = 0; i < PLAYER_COUNT - 1; i++) {
	  /* 行動を考える */
	  if (targetX < 0 && targetY < 0)
	  {
		  // 近くにいたら攻撃
		  // 自分から敵までのターン距離
		  TimeLength timeLengthMeToEnemy1 = GetTimeLength(
			    pCurrentMyPlayerData->pos.x,
			    pCurrentMyPlayerData->pos.y,
			    pCurrentMyPlayerData->angle,
			    pEnemyPlayerData[i]->pos.x,
			    pEnemyPlayerData[i]->pos.y
		      );
		  // 敵から自分までのターン距離
		  TimeLength timeLengthEnemy1ToMe = GetTimeLength(
			    pEnemyPlayerData[i]->pos.x,
			    pEnemyPlayerData[i]->pos.y,
			    pEnemyPlayerData[i]->angle,
			    pCurrentMyPlayerData->pos.x,
			    pCurrentMyPlayerData->pos.y
		      );
		  fprintf(logFp, "vs%d:		%d - %d\n", i + 1, timeLengthMeToEnemy1.turn, timeLengthEnemy1ToMe.turn);
		  // ある程度近くにいたら かつ向きがあっていたら
		  if(timeLengthMeToEnemy1.turn <= ATTACK_THRESHOLD_TURN &&
		      canAttack(
			    pCurrentMyPlayerData->pos.x,
			    pCurrentMyPlayerData->pos.y,
			    pCurrentMyPlayerData->angle,
			    pEnemyPlayerData[i]->pos.x,
			    pEnemyPlayerData[i]->pos.y
		        )
		      )
		  {
			  command->action = GameAction::Attack;
			  // 攻撃すべき状況か
			  if (pEnemyPlayerData[i]->coin > pCurrentMyPlayerData->coin && pEnemyPlayerData[i]->stunTime == 0 && timeLengthMeToEnemy1.turn <= timeLengthEnemy1ToMe.turn)
			  {
				  targetX = pEnemyPlayerData[i]->pos.x;
				  targetY = pEnemyPlayerData[i]->pos.y;
				  fprintf(logFp, "attack %d\n", i + 1);
			  }
		  }
	  }
  }

	/* 一番近いコインを検索 */
	if (targetX < 0 && targetY < 0)
	{
		// 終了までに到着しないとダメ
		int minTurn = MAX_TURN - turnData.turn;
		minTurn /= PLAYER_COUNT;
		int minCoinIndex = -1;
		for (int i = 0; i < turnData.coinCount; i++)
		{
			fprintf(logFp, "coint	%d\n", i);
			CoinData *pCurrentCoinData = &turnData.coinList[i];
			bool bContinue = false;
			for (int j_enemy = 0; j_enemy < PLAYER_COUNT - 1; j_enemy++) {
			  // 敵とくっついてたら
			  if (getLengthSquare(pCurrentMyPlayerData->pos.x, pCurrentMyPlayerData->pos.y, pEnemyPlayerData[j_enemy]->pos.x, pEnemyPlayerData[j_enemy]->pos.y) < PLAYER_RADIUS * PLAYER_RADIUS * 2)
			  {
				  // 相手にぶつかる場合は除外
				  if (
					    getLengthSquare(pCurrentMyPlayerData->pos.x, pCurrentMyPlayerData->pos.y, pCurrentCoinData->pos.x, pCurrentCoinData->pos.y) + PLAYER_RADIUS * PLAYER_RADIUS * 2 >
					    getLengthSquare(pEnemyPlayerData[j_enemy]->pos.x, pEnemyPlayerData[j_enemy]->pos.y, pCurrentCoinData->pos.x, pCurrentCoinData->pos.y
						    )
				     )
				  {
				    bContinue = true;
					  break;
				  }
			  }
			}
			if (bContinue) {
			  continue;
			}

			// ターン距離を取得する
			TimeLength timeLength = GetTimeLength(
				pCurrentMyPlayerData->pos.x,
				pCurrentMyPlayerData->pos.y,
				pCurrentMyPlayerData->angle,
				pCurrentCoinData->pos.x,
				pCurrentCoinData->pos.y
			  );
			// 現れていない取得することができないコインは無視する
			if (pCurrentCoinData->appearTime > timeLength.turn) continue;

			// 敵の方が早くとれてしまうコインは除外
			for (int j_enemy = 0; j_enemy < PLAYER_COUNT - 1; j_enemy++) {

			  TimeLength timeLengthE1 = GetTimeLength(
				    pEnemyPlayerData[j_enemy]->pos.x,
				    pEnemyPlayerData[j_enemy]->pos.y,
				    pEnemyPlayerData[j_enemy]->angle,
				    pCurrentCoinData->pos.x,
				    pCurrentCoinData->pos.y
			      );
			  if (timeLength.turn > timeLengthE1.turn)
			  {
				  bContinue = true;
					break;
			  }
			}
			if (bContinue) {
			  continue;
			}

      // コインが集まっているほど評価値を良くする
      if (timeLength.turn < MAX_TURN)
      {
        for (int j = 0; j < turnData.coinCount; j++)
        {
          if (i == j) continue;
          CoinData *pTargetCoinData = &turnData.coinList[j];
          float length = getLengthSquare(
            pCurrentCoinData->pos.x,
            pCurrentCoinData->pos.y,
            pTargetCoinData->pos.x,
            pTargetCoinData->pos.y
          );
          if (length < 100000)
            timeLength.turn -= 10;
        }
      }

			// より近いコインかどうか判定
			if (timeLength.turn < minTurn)
			{
				minTurn = timeLength.turn;
				minCoinIndex = i;
			}
			fprintf(logFp, "%d (%d, %d)\n", timeLength.turn, minTurn, minCoinIndex);
		}
		// 取得すべきコインがあれば
		if (minCoinIndex >= 0)
		{
			CoinData *pNearestCoinData = &turnData.coinList[minCoinIndex];
			fprintf(logFp, "nearest coin : %d : %d\n", minCoinIndex, minTurn);

			targetX = pNearestCoinData->pos.x;
			targetY = pNearestCoinData->pos.y;
			fprintf(logFp, "goto coin%d : %d turn\n", minCoinIndex, minTurn);
		}
	}


	// 追跡する
	for (int j_enemy = 0; j_enemy < PLAYER_COUNT - 1; j_enemy++) {
		// ここの1 - j_enemyがあるためpEnemyPlayerDataの数は2個まで
	  if (targetX < 0 && targetY < 0 && pEnemyPlayerData[j_enemy]->coin >= pEnemyPlayerData[1 - j_enemy]->coin)
	  {
		  TimeLength timeLengthMeToEnemy1 = GetTimeLength(
			    pCurrentMyPlayerData->pos.x,
			    pCurrentMyPlayerData->pos.y,
			    pCurrentMyPlayerData->angle,
			    pEnemyPlayerData[j_enemy]->pos.x,
			    pEnemyPlayerData[j_enemy]->pos.y
		      );
		  TimeLength timeLengthEnemy1ToMe = GetTimeLength(
			    pEnemyPlayerData[j_enemy]->pos.x,
			    pEnemyPlayerData[j_enemy]->pos.y,
			    pEnemyPlayerData[j_enemy]->angle,
			    pCurrentMyPlayerData->pos.x,
			    pCurrentMyPlayerData->pos.y
		      );
		  fprintf(logFp, "vs%d:		%d - %d\n", j_enemy+1, timeLengthMeToEnemy1.turn, timeLengthEnemy1ToMe.turn);
		  // 有利な状態は追跡する
		  if (pEnemyPlayerData[j_enemy]->stunTime == 0 && pEnemyPlayerData[j_enemy]->coin >= pCurrentMyPlayerData->coin && timeLengthMeToEnemy1.turn <= timeLengthEnemy1ToMe.turn)
		  {
			  targetX = pEnemyPlayerData[j_enemy]->pos.x;
			  targetY = pEnemyPlayerData[j_enemy]->pos.y;
			  targetX += WALK_DIST_PER_TURN * PLAYER_COUNT * cos(timeLengthMeToEnemy1.angle * M_PI / 180) * timeLengthMeToEnemy1.turn;
			  targetY += WALK_DIST_PER_TURN * PLAYER_COUNT * sin(timeLengthMeToEnemy1.angle * M_PI / 180) * timeLengthMeToEnemy1.turn;
		  }
	  }
	}

	// とりあえず真ん中に行ってみる
	if (targetX < 0 && targetY < 0)
	{
		targetX = 500.0;
		targetY = 500.0;
    float minDiffLength = 10000000;
    // いろんな点で試してみる
    for (int x = 0; x <= 1000; x += 50)
    {
      for (int y = 0; y <= 1000; y += 50)
      {
        // 目的地までのターン距離を算出
        TimeLength timeLengthMeToTarget = GetTimeLength(
          pCurrentMyPlayerData->pos.x,
          pCurrentMyPlayerData->pos.y,
          pCurrentMyPlayerData->angle,
          x, y
        );
        TimeLength timeLengthEnemy1ToTarget = GetTimeLength(
          pEnemyPlayer1Data->pos.x,
          pEnemyPlayer1Data->pos.y,
          pEnemyPlayer1Data->angle,
          x, y
        );
        TimeLength timeLengthEnemy2ToTarget = GetTimeLength(
          pEnemyPlayer2Data->pos.x,
          pEnemyPlayer2Data->pos.y,
          pEnemyPlayer2Data->angle,
          x, y
        );

        // 不利な目的地は選択しない
        if (timeLengthMeToTarget.turn > timeLengthEnemy1ToTarget.turn || timeLengthMeToTarget.turn > timeLengthEnemy2ToTarget.turn)
        {
          continue;
        }

        // 敵より奥の目標値は除外
        if (
					getLengthSquare(pCurrentMyPlayerData->pos.x, pCurrentMyPlayerData->pos.y, x, y) + 6400 * 2	>
					getLengthSquare(pEnemyPlayer1Data->pos.x, pEnemyPlayer1Data->pos.y, x, y) ||
					getLengthSquare(pCurrentMyPlayerData->pos.x, pCurrentMyPlayerData->pos.y, x, y) + 6400 * 2	>
					getLengthSquare(pEnemyPlayer2Data->pos.x, pEnemyPlayer2Data->pos.y, x, y)
          )
        {
          continue;
        }

        float diffLength =
					 - getLengthSquare(pCurrentMyPlayerData->pos.x, pCurrentMyPlayerData->pos.y, x, y)
					 + getLengthSquare(pEnemyPlayer1Data->pos.x, pEnemyPlayer1Data->pos.y, x, y)
					 - getLengthSquare(pCurrentMyPlayerData->pos.x, pCurrentMyPlayerData->pos.y, x, y)
					 + getLengthSquare(pEnemyPlayer2Data->pos.x, pEnemyPlayer2Data->pos.y, x, y);
        if (minDiffLength > diffLength)
        {
          minDiffLength = diffLength;
          targetX = x;
          targetY = y;
        }
      }
    }
    fprintf(logFp, "run away : minDiffLnegth = %f\n", minDiffLength);
	}

	// 指定したターゲットに向かう
	fprintf(logFp, "target (%f, %f)\n", targetX, targetY);
	angle = 180 / M_PI * atan2f( targetY - pCurrentMyPlayerData->pos.y, targetX - pCurrentMyPlayerData->pos.x) - pCurrentMyPlayerData->angle;

	// -180 ~ 180 に調整
	angle = getDiffAngle(
	    pCurrentMyPlayerData->angle,
	    pCurrentMyPlayerData->pos.x,
	    pCurrentMyPlayerData->pos.y,
	    targetX,
	    targetY,
	    DIFF_ANGLE_WITHIN_180
	    );

	fprintf(logFp, "%d\n", angle);

	// ±12度以内に収める
	angle = adjustRange(angle);

	command->angle = angle;

	// クールタイム中は攻撃できない
	if (pCurrentMyPlayerData->coolTime > 0)
	{
		command->action = GameAction::Move;
	}
	// スタンタイム中は移動も攻撃も向き変更もできない
	if (pCurrentMyPlayerData->stunTime > 0)
	{
		command->angle = 0;
		command->action = GameAction::Move;
	}

	return *command;
}
