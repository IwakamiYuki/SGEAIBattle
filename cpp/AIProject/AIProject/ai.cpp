#include<iostream>
#include "AIInterface.h"
// #include "SampleAi.h"
#include "GriphoneAi.h"

using namespace std;

class AICore {
public:
    TurnData input() {
        TurnData turnData = TurnData();

        scanf("%d %d\n", &turn, &player_id);
        scanf("%d\n", &player_count);
        turnData.turn = turn;
        turnData.myId = player_id;

        for (int i = 0; i < player_count; ++i) {
            PlayerData playerData = PlayerData();
            char stateChar[4];
            scanf("%d %f %f %d %d %s %d %d %d\n",
                  &playerData.playerId,
                  &playerData.pos.x,
                  &playerData.pos.y,
                  &playerData.angle,
                  &playerData.coin,
                  stateChar,
                  &playerData.coolTime,
                  &playerData.stunTime,
                  &playerData.lastAttackId
                  );
            if (strcmp(stateChar, "ATK")) {
                playerData.state = State::Attack;
            } else if (strcmp(stateChar, "STN")) {
                playerData.state = State::Stun;
            } else if (strcmp(stateChar, "NML")) {
                playerData.state = State::Normal;
            }
            turnData.playerList.push_back(playerData);
        }
        scanf("%d\n", &coin_count);
        turnData.coinCount = coin_count;
        for (int i = 0; i < coin_count; ++i) {
            CoinData coinData = CoinData();
            scanf("%d %d %f %f %d %f %f\n",
                  &coinData.coinId,
                  &dummy_int,
                  &coinData.pos.x,
                  &coinData.pos.y,
                  &coinData.appearTime,
                  &coinData.releasePos.x,
                  &coinData.releasePos.y
                  );
            turnData.coinList.push_back(coinData);

        }
        scanf("%s", end_text);

        return turnData;
    }

    void output(Command command) {
        printf("%d %c\n", command.angle, (command.action == GameAction::Attack ? 'A' : 'N'));
        printf("END\n");
        fflush(stdout);
        return ;
    }
private:
    int turn;
    int player_id;
    int player_count;
    int dummy_int;
    float dummy_float;
    int coin_count;
    char status[4];
    char end_text[4];
};

int main()
{
    AICore* ai_core = new AICore();
    //AIInterface* ai = new SampleAI();
    AIInterface* ai = new GriphoneAI();


    int turn = 0;
    do {
      ai->logFp = fopen("ai.log", "a");
      ai->logLocal = fopen("attack.log", "a");
		  TurnData turnData = ai_core->input();
		  Command command = ai->Update(turnData);
		  ai_core->output(command);
		  fclose(ai->logFp);
		  fclose(ai->logLocal);
    } while(turn < 600);

	 return 0;
}



