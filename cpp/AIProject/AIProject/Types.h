#ifndef __AIBattle__Types__
#define __AIBattle__Types__

#include <vector>

namespace GameAction {
    typedef enum GameAction {
        Move,
        Attack
    }GameAction;
}

namespace State {
    typedef enum State {
        Normal,
        Attack,
        Stun
    }State;
}

class Point {
public:
    float x;
    float y;
    
    Point(){x = 0;y = 0;};
    Point(float x, float y) {
        this->x = x;
        this->y = y;
    }
};

typedef struct Command {
    GameAction::GameAction action;
    int angle;
}Command;

class CoinData {
public:
    int coinId;
    Point pos;
    int appearTime;
    Point releasePos;
    CoinData(){};
};

class PlayerData {
public:
    int playerId;
    Point pos;
    int angle;
    int coin;
    State::State state;
    int coolTime;
    int stunTime;
    int lastAttackId;
    PlayerData(){};
};

class TurnData {
public:
    int turn;
    int myId;
    std::vector<PlayerData> playerList;
    std::vector<CoinData> coinList;
    
    PlayerData GetMyPlayerData() {
        return playerList.at(myId);
    }
};

#endif