#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Monster.h"
#include "Dice.h" // Chứa struct DieInfo
#include "BattleConfig.h"

enum class BattleState {
    START_ROUND,     // Random xúc xắc
    WAIT_INPUT,      // Chờ người chơi bấm 1-2-3-4
    EXECUTING,       // Đang chạy animation hành động
    CHECK_END,       // Kiểm tra chết/hết xúc xắc
    FINISHED
};

class Battle {
public:
    Battle();
    
    void loadStage(int idx);
    void loadBoss();
    void setAllies(const std::vector<Monster>& a);

    void startRound(); // Random 4 xúc xắc mới
    void update(float dt);
    void render(sf::RenderWindow& win);

    // Input: Người chơi chọn xúc xắc số [0..3]
    void playerSelectDie(int index);

    bool isFinished() const;
    bool playerWon() const;
    BattleState getState() const;

private:
    // Hàm xử lý logic khi chọn xúc xắc
    void executeDieEffect(const DieInfo& die);
    void rotateTeam(bool isPlayerTeam, bool left);
    
    // Hàm vẽ UI xúc xắc
    void renderDicePool(sf::RenderWindow& win);

    void calculateFormationPositions(bool isPlayerTeam);

    // Animation Struct
    struct AnimatedMonster {
        Monster monster;
        sf::Vector2f currentPos;
        sf::Vector2f targetPos;
        float moveTime;
        float moveDuration;
    };
    std::vector<AnimatedMonster> animatedAllies;
    std::vector<AnimatedMonster> animatedEnemies;

    std::vector<Monster> allies;
    std::vector<Monster> enemies;
    
    // --- CƠ CHẾ MỚI: DICEFOLK STYLE ---
    std::vector<DieInfo> dicePool; // Chứa 4 xúc xắc
    bool isAllyTurn;               // True: Lượt phe ta, False: Lượt phe địch
    // ----------------------------------

    // Logic queue & timer
    float actionTimer;
    BattleState state;
    bool winFlag;

    // Assets
    sf::Font font;
    sf::Text infoText;
    sf::Texture backgroundTex;
    sf::Sprite backgroundSprite;
};