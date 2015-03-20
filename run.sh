#!/bin/sh
name1=`date '+%Y_%m_%d_%H_%M_%S'`
name2=`date '-v+1S' '+%Y_%m_%d_%H_%M_%S'`
rm ai.log
java -jar arena.jar -cpp x.out -cpp cpp/AIProject/DerivedData/AIProject/Build/Products/Debug/AIProject -cpp cpp/AIProject/DerivedData/AIProject/Build/Products/Debug/AIProject
cp Replay/replay_$name1.json /Users/iwakamiyuki/Downloads/AIBattle/Player/Assets/Resources/GameData/replay.json
cp Replay/replay_$name2.json /Users/iwakamiyuki/Downloads/AIBattle/Player/Assets/Resources/GameData/replay.json
