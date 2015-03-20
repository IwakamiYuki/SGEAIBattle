#include<iostream>
#include<stdlib.h>
#include "SampleAI.h"

using namespace std;

SampleAI::SampleAI() {
    srand((unsigned) 1);
}

Command SampleAI::Update(TurnData turnData) {
    Command* command = new Command();
	
	fprintf(logFp, "turn:%d********************\n", turnData.turn);
    for (int i = 0; i < 3; i++)
    {
        fprintf(logFp, "player%d - - - - - - - - - - \n", i);
        PlayerData *pCurrentPlayerData = &turnData.playerList[i];
        fprintf(logFp, "pos.x:        %f\n", pCurrentPlayerData->pos.x);
        fprintf(logFp, "pos.y:        %f\n", pCurrentPlayerData->pos.y);
        fprintf(logFp, "angle:        %d\n", pCurrentPlayerData->angle);
        fprintf(logFp, "coin:         %d\n", pCurrentPlayerData->coin);
        fprintf(logFp, "state:        %d\n", pCurrentPlayerData->state);
        fprintf(logFp, "coolTime:     %d\n", pCurrentPlayerData->coolTime);
        fprintf(logFp, "stunTime:     %d\n", pCurrentPlayerData->stunTime);
        fprintf(logFp, "lastAttackId: %d\n", pCurrentPlayerData->lastAttackId);
    }
    

    /* action is Attack or Move */
    if (turnData.turn / 3 % 40 == 0) {
        command->action = GameAction::Attack;
    }
    else {
        command->action = GameAction::Move;
    }
    
    /* -12 <= angle <= +12 */
    command->angle = rand() % 25 - 12;
    
    return *command;
}
