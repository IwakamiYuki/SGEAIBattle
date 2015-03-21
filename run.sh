#!/bin/sh
rm ai.log
# 過去ログを移動
mv Replay/* Replay_back
# コンパイル
g++ cpp/AIProject/AIProject/Moriken001Ai.cpp cpp/AIProject/AIProject/ai.cpp -o cpp/AIProject/AIProject/x.out
# 実行
java -jar arena.jar -cpp cpp/AIProject/AIProject/x.out -cpp getNearestCoin.out -cpp cpp/AIProject/DerivedData/AIProject/Build/Products/Debug/AIProject
# JSONデータをPlayerにコピー
find Replay -name "replay_*.json" -exec cp {} ~/Downloads/AiBattle/Player/Assets/Resources/GameData/replay.json \;
# Unity再生
open ~/Downloads/AiBattle/Player/Assets/Scenes/MainScene.unity
