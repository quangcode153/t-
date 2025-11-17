#pragma once
#include <vector>
#include "Monster.h"

struct StageInfo {
    std::vector<Monster> enemies;
};

class BattleConfig {
public:
    static std::vector<StageInfo> stages;
    static StageInfo boss;

    static void initStages();
};