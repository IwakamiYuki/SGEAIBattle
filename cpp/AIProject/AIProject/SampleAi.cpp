#include<iostream>
#include<stdlib.h>
#include "SampleAI.h"

using namespace std;

SampleAI::SampleAI() {
    srand((unsigned) time(NULL));
}

Command SampleAI::Update(TurnData turnData) {
    Command* command = new Command();
    
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
