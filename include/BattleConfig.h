#pragma once
#include <vector>
#include "Monster.h"

struct StageInfo {
    std::vector<Monster> enemies;
};

class BattleConfig {
public:
    // Khai báo static để truy cập toàn cục
    static std::vector<StageInfo> stages;
    static StageInfo boss;

    static void initStages();
};