
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
		dAngle = (dAngle + 720 + 180) % 360 - 180;
		int ddAngle = dAngle;

		// ±12度以内に収める
		if (dAngle > MAX_RANGE) dAngle = MAX_RANGE;
		if (dAngle < -MAX_RANGE) dAngle = -MAX_RANGE;

		// 移動する
		currentAngle += dAngle;
		currentX += MOVEMENT * cos(currentAngle * M_PI / 180);
		currentY += MOVEMENT * sin(currentAngle * M_PI / 180);

		// ある程度まで近づいたら目的地に到着とする
		if (getLengthSquare(currentX, currentY, targetX, targetY) < 10000)
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
		TimeLength timeLengthMtoE1 = GetTimeLength(
			pCurrentMyPlayerData->pos.x,
			pCurrentMyPlayerData->pos.y,
			pCurrentMyPlayerData->angle,
			pEnemyPlayer1Data->pos.x,
			pEnemyPlayer1Data->pos.y
		);
		// 敵から自分までのターン距離
		TimeLength timeLengthE1toM = GetTimeLength(
			pEnemyPlayer1Data->pos.x,
			pEnemyPlayer1Data->pos.y,
			pEnemyPlayer1Data->angle,
			pCurrentMyPlayerData->pos.x,
			pCurrentMyPlayerData->pos.y
		);
		fprintf(logFp, "vs1:		%d - %d\n", timeLengthMtoE1.turn, timeLengthE1toM.turn);
		// ある程度近くにいたら
		if (timeLengthMtoE1.turn <= PLAYER_COUNT)
		{
			command->action = GameAction::Attack;
			// 攻撃すべき状況か
			if (pEnemyPlayer1Data->coin > pCurrentMyPlayerData->coin && pEnemyPlayer1Data->stunTime == 0 && timeLengthMtoE1.turn <= timeLengthE1toM.turn)
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
		TimeLength timeLengthMtoE2 = GetTimeLength(
			pCurrentMyPlayerData->pos.x,
			pCurrentMyPlayerData->pos.y,
			pCurrentMyPlayerData->angle,
			pEnemyPlayer2Data->pos.x,
			pEnemyPlayer2Data->pos.y
		);
		// 敵から自分までのターン距離
		TimeLength timeLengthE2toM = GetTimeLength(
			pEnemyPlayer2Data->pos.x,
			pEnemyPlayer2Data->pos.y,
			pEnemyPlayer2Data->angle,
			pCurrentMyPlayerData->pos.x,
			pCurrentMyPlayerData->pos.y
		);
		fprintf(logFp, "vs2:		%d - %d\n", timeLengthMtoE2.turn, timeLengthE2toM.turn);
		// ある程度近くにいたら
		if(timeLengthMtoE2.turn <= PLAYER_COUNT)
		{
			command->action = GameAction::Attack;
			// 攻撃すべき状況か
			if (pEnemyPlayer2Data->coin > pCurrentMyPlayerData->coin && pEnemyPlayer2Data->stunTime == 0 && timeLengthMtoE2.turn <= timeLengthE2toM.turn)
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
		TimeLength timeLengthMtoE1 = GetTimeLength(
			pCurrentMyPlayerData->pos.x,
			pCurrentMyPlayerData->pos.y,
			pCurrentMyPlayerData->angle,
			pEnemyPlayer1Data->pos.x,
			pEnemyPlayer1Data->pos.y
		);
		TimeLength timeLengthE1toM = GetTimeLength(
			pEnemyPlayer1Data->pos.x,
			pEnemyPlayer1Data->pos.y,
			pEnemyPlayer1Data->angle,
			pCurrentMyPlayerData->pos.x,
			pCurrentMyPlayerData->pos.y
		);
		fprintf(logFp, "vs1:		%d - %d\n", timeLengthMtoE1.turn, timeLengthE1toM.turn);
		// 有利な状態は追跡する
		if (pEnemyPlayer1Data->stunTime == 0 && pEnemyPlayer1Data->coin >= pCurrentMyPlayerData->coin && timeLengthMtoE1.turn <= timeLengthE1toM.turn)
		{
			targetX = pEnemyPlayer1Data->pos.x;
			targetY = pEnemyPlayer1Data->pos.y;
			targetX += MOVEMENT * cos(timeLengthMtoE1.angle * M_PI / 180) * timeLengthMtoE1.turn;
			targetY += MOVEMENT * sin(timeLengthMtoE1.angle * M_PI / 180) * timeLengthMtoE1.turn;
		}
	}

	if (targetX < 0 && targetY < 0 && pEnemyPlayer2Data->coin >= pEnemyPlayer1Data->coin)
	{
		TimeLength timeLengthMtoE2 = GetTimeLength(
			pCurrentMyPlayerData->pos.x,
			pCurrentMyPlayerData->pos.y,
			pCurrentMyPlayerData->angle,
			pEnemyPlayer2Data->pos.x,
			pEnemyPlayer2Data->pos.y
		);
		TimeLength timeLengthE2toM = GetTimeLength(
			pEnemyPlayer2Data->pos.x,
			pEnemyPlayer2Data->pos.y,
			pEnemyPlayer2Data->angle,
			pCurrentMyPlayerData->pos.x,
			pCurrentMyPlayerData->pos.y
		);
		fprintf(logFp, "vs2:		%d - %d\n", timeLengthMtoE2.turn, timeLengthE2toM.turn);
		// 有利な状態は追跡する
		if (pEnemyPlayer2Data->stunTime == 0 && pEnemyPlayer2Data->coin >= pCurrentMyPlayerData->coin && timeLengthMtoE2.turn <= timeLengthE2toM.turn)
		{
			targetX = pEnemyPlayer2Data->pos.x;
			targetY = pEnemyPlayer2Data->pos.y;
			targetX += MOVEMENT * cos(timeLengthMtoE2.angle * M_PI / 180) * timeLengthMtoE2.turn;
			targetY += MOVEMENT * sin(timeLengthMtoE2.angle * M_PI / 180) * timeLengthMtoE2.turn;
		}
	}

	// とりあえず真ん中に行ってみる
	if (targetX < 0 && targetY < 0)
	{
		targetX = 500.0;
		targetY = 500.0;
	}

	// 指定したターゲットに向かう
	fprintf(logFp, "target (%f, %f)\n", targetX, targetY);
	angle = 180 / M_PI * atan2f( targetY - pCurrentMyPlayerData->pos.y, targetX - pCurrentMyPlayerData->pos.x) - pCurrentMyPlayerData->angle;

	// -180 ~ 180 に調整
	angle = (angle + 720 + 180) % 360 - 180;
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

