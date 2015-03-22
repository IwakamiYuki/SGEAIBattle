
#include "GriphoneAI.h"

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
		int dAngle = 180 / M_PI * atan2f(targetY - currentY, targetX - currentX) - currentAngle;

		// -180 ~ 180 に調整
		dAngle = adjustAngle(dAngle);
		int ddAngle = dAngle;

		// ±12度以内に収める
		if (dAngle > MAX_RANGE) dAngle = MAX_RANGE;
		if (dAngle < -MAX_RANGE) dAngle = -MAX_RANGE;

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

Command GriphoneAI::Update(TurnData turnData)
{
	Command* command = new Command();

	fprintf(logFp, "turn:%d********************\n", turnData.turn);
	fprintf(logFp, "myId:%d\n", turnData.myId);
	for (int i = 0; i < 3; i++)
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
	PlayerData *pEnemyPlayer1Data = &turnData.playerList[(turnData.myId + 1) % 3];
	PlayerData *pEnemyPlayer2Data = &turnData.playerList[(turnData.myId + 2) % 3];


	/* 行動を考える */
	if (targetX < 0 && targetY < 0)
	{
		// 近くにいたら攻撃
		// 自分から敵までのターン距離
		TimeLength timeLengthMeToEnemy1 = GetTimeLength(
			pCurrentMyPlayerData->pos.x,
			pCurrentMyPlayerData->pos.y,
			pCurrentMyPlayerData->angle,
			pEnemyPlayer1Data->pos.x,
			pEnemyPlayer1Data->pos.y
		);
		// 敵から自分までのターン距離
		TimeLength timeLengthEnemy1ToMe = GetTimeLength(
			pEnemyPlayer1Data->pos.x,
			pEnemyPlayer1Data->pos.y,
			pEnemyPlayer1Data->angle,
			pCurrentMyPlayerData->pos.x,
			pCurrentMyPlayerData->pos.y
		);
		fprintf(logFp, "vs1:		%d - %d\n", timeLengthMeToEnemy1.turn, timeLengthEnemy1ToMe.turn);
		// ある程度近くにいたら
		if (timeLengthMeToEnemy1.turn <= PLAYER_COUNT)
		{
			command->action = GameAction::Attack;
			// 攻撃すべき状況か
			if (pEnemyPlayer1Data->coin > pCurrentMyPlayerData->coin && pEnemyPlayer1Data->stunTime == 0 && timeLengthMeToEnemy1.turn <= timeLengthEnemy1ToMe.turn)
			{
				targetX = pEnemyPlayer1Data->pos.x;
				targetY = pEnemyPlayer1Data->pos.y;
				fprintf(logFp, "attack 2\n");
			}
		}
	}
	if (targetX < 0 && targetY < 0)
	{
		// 近くにいたら攻撃
		// 自分から敵までのターン距離
		TimeLength timeLengthMeToEnemy2 = GetTimeLength(
			pCurrentMyPlayerData->pos.x,
			pCurrentMyPlayerData->pos.y,
			pCurrentMyPlayerData->angle,
			pEnemyPlayer2Data->pos.x,
			pEnemyPlayer2Data->pos.y
		);
		// 敵から自分までのターン距離
		TimeLength timeLengthEnemy2ToMe = GetTimeLength(
			pEnemyPlayer2Data->pos.x,
			pEnemyPlayer2Data->pos.y,
			pEnemyPlayer2Data->angle,
			pCurrentMyPlayerData->pos.x,
			pCurrentMyPlayerData->pos.y
		);
		fprintf(logFp, "vs2:		%d - %d\n", timeLengthMeToEnemy2.turn, timeLengthEnemy2ToMe.turn);
		// ある程度近くにいたら
		if(timeLengthMeToEnemy2.turn <= PLAYER_COUNT)
		{
			command->action = GameAction::Attack;
			// 攻撃すべき状況か
			if (pEnemyPlayer2Data->coin > pCurrentMyPlayerData->coin && pEnemyPlayer2Data->stunTime == 0 && timeLengthMeToEnemy2.turn <= timeLengthEnemy2ToMe.turn)
			{
				targetX = pEnemyPlayer2Data->pos.x;
				targetY = pEnemyPlayer2Data->pos.y;
				fprintf(logFp, "attack 3\n");
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

			// 敵とくっついてたら
			if (getLengthSquare(pCurrentMyPlayerData->pos.x, pCurrentMyPlayerData->pos.y, pEnemyPlayer1Data->pos.x, pEnemyPlayer1Data->pos.y) < 6400 * 2)
			{
				// 相手にぶつかる場合は除外
				if (
					getLengthSquare(pCurrentMyPlayerData->pos.x, pCurrentMyPlayerData->pos.y, pCurrentCoinData->pos.x, pCurrentCoinData->pos.y) + 6400 * 2 >
					getLengthSquare(pEnemyPlayer1Data->pos.x, pEnemyPlayer1Data->pos.y, pCurrentCoinData->pos.x, pCurrentCoinData->pos.y
						)
				)
				{
					continue;
				}
			}
			// 敵とくっついてたら
			if (getLengthSquare(pCurrentMyPlayerData->pos.x, pCurrentMyPlayerData->pos.y, pEnemyPlayer2Data->pos.x, pEnemyPlayer2Data->pos.y) < 6400 * 2)
			{
				// 相手にぶつかる場合は除外
				if (
					getLengthSquare(pCurrentMyPlayerData->pos.x, pCurrentMyPlayerData->pos.y, pCurrentCoinData->pos.x, pCurrentCoinData->pos.y) + 6400 * 2	>
					getLengthSquare(pEnemyPlayer2Data->pos.x, pEnemyPlayer2Data->pos.y, pCurrentCoinData->pos.x, pCurrentCoinData->pos.y
						)
				)
				{
					continue;
				}
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
			TimeLength timeLengthE1 = GetTimeLength(
				pEnemyPlayer1Data->pos.x,
				pEnemyPlayer1Data->pos.y,
				pEnemyPlayer1Data->angle,
				pCurrentCoinData->pos.x,
				pCurrentCoinData->pos.y
			);
			if (timeLength.turn > timeLengthE1.turn)
			{
				continue;
			}
			TimeLength timeLengthE2 = GetTimeLength(
				pEnemyPlayer2Data->pos.x,
				pEnemyPlayer2Data->pos.y,
				pEnemyPlayer2Data->angle,
				pCurrentCoinData->pos.x,
				pCurrentCoinData->pos.y
			);
			if (timeLength.turn > timeLengthE2.turn)
			{
				continue;
			}

      // コインが集まっているほど評価値を良くする
      if (timeLength.turn < MAX_TURN)
      {
        float nearCoinInverseLength = 0;
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
          if (length < 100000000) // 必要ないかもだからすごい大きな値にしている
          {
            // 距離の逆数をたす
            nearCoinInverseLength += 1000 / length;
          }
        }
        // 評価が高いほどマイナス分が大きくなりよいコインとなる
        timeLength.turn -= nearCoinInverseLength;
        fprintf(logFp, "near coin point : %f\n", nearCoinInverseLength);
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
	if (targetX < 0 && targetY < 0 && pEnemyPlayer1Data->coin >= pEnemyPlayer2Data->coin)
	{
		TimeLength timeLengthMeToEnemy1 = GetTimeLength(
			pCurrentMyPlayerData->pos.x,
			pCurrentMyPlayerData->pos.y,
			pCurrentMyPlayerData->angle,
			pEnemyPlayer1Data->pos.x,
			pEnemyPlayer1Data->pos.y
		);
		TimeLength timeLengthEnemy1ToMe = GetTimeLength(
			pEnemyPlayer1Data->pos.x,
			pEnemyPlayer1Data->pos.y,
			pEnemyPlayer1Data->angle,
			pCurrentMyPlayerData->pos.x,
			pCurrentMyPlayerData->pos.y
		);
		fprintf(logFp, "vs1:		%d - %d\n", timeLengthMeToEnemy1.turn, timeLengthEnemy1ToMe.turn);
		// 有利な状態は追跡する
		if (pEnemyPlayer1Data->stunTime == 0 && pEnemyPlayer1Data->coin >= pCurrentMyPlayerData->coin && timeLengthMeToEnemy1.turn <= timeLengthEnemy1ToMe.turn)
		{
			targetX = pEnemyPlayer1Data->pos.x;
			targetY = pEnemyPlayer1Data->pos.y;
			targetX += WALK_DIST_PER_TURN * PLAYER_COUNT * cos(timeLengthMeToEnemy1.angle * M_PI / 180) * timeLengthMeToEnemy1.turn;
			targetY += WALK_DIST_PER_TURN * PLAYER_COUNT * sin(timeLengthMeToEnemy1.angle * M_PI / 180) * timeLengthMeToEnemy1.turn;
		}
	}

	if (targetX < 0 && targetY < 0 && pEnemyPlayer2Data->coin >= pEnemyPlayer1Data->coin)
	{
		TimeLength timeLengthMeToEnemy2 = GetTimeLength(
			pCurrentMyPlayerData->pos.x,
			pCurrentMyPlayerData->pos.y,
			pCurrentMyPlayerData->angle,
			pEnemyPlayer2Data->pos.x,
			pEnemyPlayer2Data->pos.y
		);
		TimeLength timeLengthEnemy2ToMe = GetTimeLength(
			pEnemyPlayer2Data->pos.x,
			pEnemyPlayer2Data->pos.y,
			pEnemyPlayer2Data->angle,
			pCurrentMyPlayerData->pos.x,
			pCurrentMyPlayerData->pos.y
		);
		fprintf(logFp, "vs2:		%d - %d\n", timeLengthMeToEnemy2.turn, timeLengthEnemy2ToMe.turn);
		// 有利な状態は追跡する
		if (pEnemyPlayer2Data->stunTime == 0 && pEnemyPlayer2Data->coin >= pCurrentMyPlayerData->coin && timeLengthMeToEnemy2.turn <= timeLengthEnemy2ToMe.turn)
		{
			targetX = pEnemyPlayer2Data->pos.x;
			targetY = pEnemyPlayer2Data->pos.y;
			targetX += WALK_DIST_PER_TURN * PLAYER_COUNT * cos(timeLengthMeToEnemy2.angle * M_PI / 180) * timeLengthMeToEnemy2.turn;
			targetY += WALK_DIST_PER_TURN * PLAYER_COUNT * sin(timeLengthMeToEnemy2.angle * M_PI / 180) * timeLengthMeToEnemy2.turn;
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
	angle = adjustAngle(angle);
	fprintf(logFp, "%d\n", angle);

	// ±12度以内に収める
	if (angle > MAX_RANGE) angle = MAX_RANGE;
	if (angle < -MAX_RANGE) angle = -MAX_RANGE;

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

