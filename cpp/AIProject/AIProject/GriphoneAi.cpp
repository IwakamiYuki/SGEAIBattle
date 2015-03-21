
#include "GriphoneAI.h"

using namespace std;

GriphoneAI::GriphoneAI()
{
	srand((unsigned) time(NULL));
}

TimeLength GriphoneAI::GetTimeLength(float startX, float startY, int startAngle, float targetX, float targetY)
{
	TimeLength* timeLength = new TimeLength();

	float currentX = startX;
	float currentY = startY;
	int currentAngle = startAngle;

  timeLength->turn = 6000;
	//*
	for (int i = 1; i < 600; i++)
	{
		int dAngle = 180 / M_PI * atan2f(targetY - currentY, targetX - currentX) - currentAngle;

		dAngle = (dAngle + 720 + 180) % 360 - 180;
		int ddAngle = dAngle;

		if (dAngle > 12) dAngle = 12;
		if (dAngle < -12) dAngle = -12;

		currentAngle += dAngle;
		currentX += 30 * cos(currentAngle * M_PI / 180);
		currentY += 30 * sin(currentAngle * M_PI / 180);

		if ((currentX - targetX) * (currentX - targetX) + (currentY - targetY) * (currentY - targetY) < 1000)
		{
			fprintf(logFp, "cul %d:	(%f,%f, %d)=>(%f,%f)	%d \n", i, currentX, currentY, currentAngle, targetX, targetY, ddAngle);
			timeLength->turn = i;
			timeLength->angle = currentAngle;
			break;
		}
	}
	/*/
	float turn = (startX - targetX) * (startX - targetX) + (startY - targetY) * (startY - targetY);
	timeLength->turn = int(sqrt(turn)) / 30;
	//*/
	return *timeLength;
}

float GriphoneAI::getLengthSquare(float x1, float y1, float x2, float y2)
{
  return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
}

Command GriphoneAI::Update(TurnData turnData) {
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



	// 初期設定
	PlayerData *pCurrentMyPlayerData = &turnData.playerList[turnData.myId];

	int angle = 0;
	float targetX = -1;
	float targetY = -1;

	command->action = GameAction::Move;

	PlayerData *pEnemmyPlayer1Data = &turnData.playerList[(turnData.myId + 1) % 3];
	PlayerData *pEnemmyPlayer2Data = &turnData.playerList[(turnData.myId + 2) % 3];

	// 近くにいたら攻撃
	if (targetX < 0 && targetY < 0)
	{
		TimeLength timeLengthMtoE1 = GetTimeLength(
			pCurrentMyPlayerData->pos.x,
			pCurrentMyPlayerData->pos.y,
			pCurrentMyPlayerData->angle,
			pEnemmyPlayer1Data->pos.x,
			pEnemmyPlayer1Data->pos.y
		);
		TimeLength timeLengthE1toM = GetTimeLength(
			pEnemmyPlayer1Data->pos.x,
			pEnemmyPlayer1Data->pos.y,
			pEnemmyPlayer1Data->angle,
			pCurrentMyPlayerData->pos.x,
			pCurrentMyPlayerData->pos.y
		);
		fprintf(logFp, "vs1:		%d - %d\n", timeLengthMtoE1.turn, timeLengthE1toM.turn);
		if (pEnemmyPlayer1Data->coin > pCurrentMyPlayerData->coin && pEnemmyPlayer1Data->stunTime == 0 && timeLengthMtoE1.turn <=5 && timeLengthMtoE1.turn <= timeLengthE1toM.turn)
		{
			targetX = pEnemmyPlayer1Data->pos.x;
			targetY = pEnemmyPlayer1Data->pos.y;
			command->action = GameAction::Attack;
      fprintf(logFp, "attack 1\n");
		}
	}
	if (targetX < 0 && targetY < 0)
	{
		TimeLength timeLengthMtoE2 = GetTimeLength(
			pCurrentMyPlayerData->pos.x,
			pCurrentMyPlayerData->pos.y,
			pCurrentMyPlayerData->angle,
			pEnemmyPlayer2Data->pos.x,
			pEnemmyPlayer2Data->pos.y
		);
		TimeLength timeLengthE2toM = GetTimeLength(
			pEnemmyPlayer2Data->pos.x,
			pEnemmyPlayer2Data->pos.y,
			pEnemmyPlayer2Data->angle,
			pCurrentMyPlayerData->pos.x,
			pCurrentMyPlayerData->pos.y
		);
		fprintf(logFp, "vs2:		%d - %d\n", timeLengthMtoE2.turn, timeLengthE2toM.turn);
		if (pEnemmyPlayer2Data->coin > pCurrentMyPlayerData->coin && pEnemmyPlayer2Data->stunTime == 0 && timeLengthMtoE2.turn <=5 && timeLengthMtoE2.turn <= timeLengthE2toM.turn)
		{
			targetX = pEnemmyPlayer2Data->pos.x;
			targetY = pEnemmyPlayer2Data->pos.y;
			command->action = GameAction::Attack;
      fprintf(logFp, "attack 2\n");
		}
	}



	/* 一番近いコインを検索 */
	if (targetX < 0 && targetY < 0)
	{
		//int minTurn = 999999999;
		int minTurn = 1800 - turnData.turn;
    minTurn /= 3;
		int minCoinIndex = -1;
		for (int i = 0; i < turnData.coinCount; i++)
		{
      fprintf(logFp, "coint	%d\n", i);
			CoinData *pCurrentCoinData = &turnData.coinList[i];

      // 敵とくっついてたら
      if (getLengthSquare(pCurrentMyPlayerData->pos.x, pCurrentMyPlayerData->pos.y, pEnemmyPlayer1Data->pos.x, pEnemmyPlayer1Data->pos.y) < 6400)
      {
        if (
          getLengthSquare(pCurrentMyPlayerData->pos.x, pCurrentMyPlayerData->pos.y, pCurrentCoinData->pos.x, pCurrentCoinData->pos.y) + 6400 >
          getLengthSquare(pEnemmyPlayer1Data->pos.x, pEnemmyPlayer1Data->pos.y, pCurrentCoinData->pos.x, pCurrentCoinData->pos.y
            )
        )
        {
          continue;
        }
      }
      // 敵とくっついてたら
      if (getLengthSquare(pCurrentMyPlayerData->pos.x, pCurrentMyPlayerData->pos.y, pEnemmyPlayer2Data->pos.x, pEnemmyPlayer2Data->pos.y) < 6400)
      {
        if (
          getLengthSquare(pCurrentMyPlayerData->pos.x, pCurrentMyPlayerData->pos.y, pCurrentCoinData->pos.x, pCurrentCoinData->pos.y) + 6400  >
          getLengthSquare(pEnemmyPlayer2Data->pos.x, pEnemmyPlayer2Data->pos.y, pCurrentCoinData->pos.x, pCurrentCoinData->pos.y
            )
        )
        {
          continue;
        }
      }

			// 距離を取得する
			TimeLength timeLength = GetTimeLength(
				pCurrentMyPlayerData->pos.x,
				pCurrentMyPlayerData->pos.y,
				pCurrentMyPlayerData->angle,
				pCurrentCoinData->pos.x,
				pCurrentCoinData->pos.y
			);
			// 現れてないコインは無視する
			if (pCurrentCoinData->appearTime > timeLength.turn) continue;

			PlayerData *pEnemmyPlayer1Data = &turnData.playerList[(turnData.myId + 1) % 3];
			TimeLength timeLengthE1 = GetTimeLength(
				pEnemmyPlayer1Data->pos.x,
				pEnemmyPlayer1Data->pos.y,
				pEnemmyPlayer1Data->angle,
				pCurrentCoinData->pos.x,
				pCurrentCoinData->pos.y
			);
			if (timeLength.turn > timeLengthE1.turn)
			{
				continue;
			}
			PlayerData *pEnemmyPlayer2Data = &turnData.playerList[(turnData.myId + 2) % 3];
			TimeLength timeLengthE2 = GetTimeLength(
				pEnemmyPlayer2Data->pos.x,
				pEnemmyPlayer2Data->pos.y,
				pEnemmyPlayer2Data->angle,
				pCurrentCoinData->pos.x,
				pCurrentCoinData->pos.y
			);
			if (timeLength.turn > timeLengthE2.turn)
			{
				continue;
			}


			if (timeLength.turn < minTurn)
			{
				minTurn = timeLength.turn;
				minCoinIndex = i;
			}
			fprintf(logFp, "%d (%d, %d)\n", timeLength.turn, minTurn, minCoinIndex);
		}
		if (minCoinIndex >= 0)
		{
			CoinData *pNearestCoinData = &turnData.coinList[minCoinIndex];
			fprintf(logFp, "nearest coin : %d : %d\n", minCoinIndex, minTurn);

			targetX = pNearestCoinData->pos.x;
			targetY = pNearestCoinData->pos.y;
      fprintf(logFp, "goto coin%d : %d turn\n", minCoinIndex, minTurn);

			/*
			if (pNearestCoinData->pos.y - pCurrentMyPlayerData->pos.y != 0)
			angle = 180 / M_PI * atan2f( pNearestCoinData->pos.y - pCurrentMyPlayerData->pos.y, pNearestCoinData->pos.x - pCurrentMyPlayerData->pos.x) - pCurrentMyPlayerData->angle;

			angle = (angle + 720 + 180) % 360 -180;
			fprintf(logFp, "%d\n", angle);

			if (angle > 12) angle = 12;
			if (angle < -12) angle = -12;
			*/
		}
	}

	if (targetX < 0 && targetY < 0 && pEnemmyPlayer1Data->coin >= pEnemmyPlayer2Data->coin)
	{
		TimeLength timeLengthMtoE1 = GetTimeLength(
			pCurrentMyPlayerData->pos.x,
			pCurrentMyPlayerData->pos.y,
			pCurrentMyPlayerData->angle,
			pEnemmyPlayer1Data->pos.x,
			pEnemmyPlayer1Data->pos.y
		);
		TimeLength timeLengthE1toM = GetTimeLength(
			pEnemmyPlayer1Data->pos.x,
			pEnemmyPlayer1Data->pos.y,
			pEnemmyPlayer1Data->angle,
			pCurrentMyPlayerData->pos.x,
			pCurrentMyPlayerData->pos.y
		);
		fprintf(logFp, "vs1:		%d - %d\n", timeLengthMtoE1.turn, timeLengthE1toM.turn);
		if (pEnemmyPlayer1Data->stunTime == 0 && pEnemmyPlayer1Data->coin >= pCurrentMyPlayerData->coin && timeLengthMtoE1.turn <= timeLengthE1toM.turn)
		{
			targetX = pEnemmyPlayer1Data->pos.x;
			targetY = pEnemmyPlayer1Data->pos.y;
      targetX += 30 * cos(timeLengthMtoE1.angle * M_PI / 180) * timeLengthMtoE1.turn;
      targetY += 30 * sin(timeLengthMtoE1.angle * M_PI / 180) * timeLengthMtoE1.turn;
		}
	}

	if (targetX < 0 && targetY < 0 && pEnemmyPlayer2Data->coin >= pEnemmyPlayer1Data->coin)
	{
		TimeLength timeLengthMtoE2 = GetTimeLength(
			pCurrentMyPlayerData->pos.x,
			pCurrentMyPlayerData->pos.y,
			pCurrentMyPlayerData->angle,
			pEnemmyPlayer2Data->pos.x,
			pEnemmyPlayer2Data->pos.y
		);
		TimeLength timeLengthE2toM = GetTimeLength(
			pEnemmyPlayer2Data->pos.x,
			pEnemmyPlayer2Data->pos.y,
			pEnemmyPlayer2Data->angle,
			pCurrentMyPlayerData->pos.x,
			pCurrentMyPlayerData->pos.y
		);
		fprintf(logFp, "vs2:		%d - %d\n", timeLengthMtoE2.turn, timeLengthE2toM.turn);
		if (pEnemmyPlayer2Data->stunTime == 0 && pEnemmyPlayer2Data->coin >= pCurrentMyPlayerData->coin && timeLengthMtoE2.turn <= timeLengthE2toM.turn)
		{
			targetX = pEnemmyPlayer2Data->pos.x;
			targetY = pEnemmyPlayer2Data->pos.y;
      targetX += 30 * cos(timeLengthMtoE2.angle * M_PI / 180) * timeLengthMtoE2.turn;
      targetY += 30 * sin(timeLengthMtoE2.angle * M_PI / 180) * timeLengthMtoE2.turn;
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

	angle = (angle + 720 + 180) % 360 - 180;
	fprintf(logFp, "%d\n", angle);

	if (angle > 12) angle = 12;
	if (angle < -12) angle = -12;


	/* action is Attack or Move */
	if (turnData.turn / 3 % 40 == 0) {
		//	command->action = GameAction::Attack;
	}
	else {
		//	command->action = GameAction::Move;
	}
	//command->action = GameAction::Move;

	/* -12 <= angle <= +12 */
	//command->angle = rand() % 25 - 12;


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
