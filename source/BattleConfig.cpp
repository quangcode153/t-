#include "BattleConfig.h"

using namespace std;

vector<StageInfo> BattleConfig::stages;
StageInfo BattleConfig::boss;

void BattleConfig::initStages() {
    stages.clear();
    stages.resize(6); // 6 màn chơi
    
    // Stage 0: Gặp Boglin yếu
    stages[0].enemies = { 
        Monster("Boglin", 15, 3, "assets/sprites/Boglin.png"), 
        Monster("Boglin", 15, 3, "assets/sprites/Boglin.png") 
    };

    // Stage 1: Gặp Beloid
    stages[1].enemies = { 
        Monster("Beloid A", 20, 4, "assets/sprites/Beloid_A.png"), 
        Monster("Beloid B", 18, 5, "assets/sprites/Beloid_B.png") 
    };

    // Stage 2: Hỗn hợp
    stages[2].enemies = { 
        Monster("Carascohl", 25, 4, "assets/sprites/Carascohl.png"), 
        Monster("Boglin", 15, 3, "assets/sprites/Boglin.png") 
    };

    // Stage 3: Địch mạnh hơn
    stages[3].enemies = { 
        Monster("Escabal", 35, 6, "assets/sprites/Escabal.png"), 
        Monster("Beloid C", 22, 5, "assets/sprites/Beloid_C.png") 
    };

    // Stage 4
    stages[4].enemies = { 
        Monster("Chimera", 40, 7, "assets/sprites/Chimera_Shrine.png"), 
        Monster("Carascohl", 25, 4, "assets/sprites/Carascohl.png") 
    };

    // Stage 5
    stages[5].enemies = { 
        Monster("Escabal", 40, 8, "assets/sprites/Escabal.png"), 
        Monster("Escabal", 40, 8, "assets/sprites/Escabal.png") 
    };
    
    // BOSS: Akama
    boss.enemies = { 
        Monster("Akama (Boss)", 200, 12, "assets/sprites/Akama.png") 
    };
}