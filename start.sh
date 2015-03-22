#!/bin/sh
rm ai.log
rm attack.log
# 過去ログを移動
mv Replay/* Replay_back
# コンパイル
g++ cpp/AIProject/AIProject/GriphoneAi.cpp cpp/AIProject/AIProject/ai.cpp -o cpp/AIProject/AIProject/x.out
# 実行
java -jar arena.jar -cpp cpp/AIProject/AIProject/x.out -cpp Iwakami003.out -cpp Iwakami002.out
# java -jar arena.jar -cpp cpp/AIProject/AIProject/x.out -cpp cpp/AIProject/AIProject/x.out -cpp cpp/AIProject/AIProject/x.out
# java -jar arena.jar -cpp cpp/AIProject/AIProject/x.out -cpp getNearestCoin.out -cpp cpp/AIProject/DerivedData/AIProject/Build/Products/Debug/AIProject
# JSONデータをPlayerにコピー
find Replay -name "replay_*.json" -exec cp {} ~/Downloads/AiBattle/Player/Assets/Resources/GameData/replay.json \;
# Unity再生
# open ~/Downloads/AiBattle/Player/Assets/Scenes/MainScene.unity
