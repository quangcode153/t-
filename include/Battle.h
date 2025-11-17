#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Monster.h"
#include "Node.h" 
#include "Dice.h" 

// --- XÓA ĐOẠN enum class GameState Ở ĐÂY NẾU CÓ ---
// (Vì nó đã nằm bên Game.h rồi)

enum class BattleState {
    START_ROUND, WAIT_INPUT, EXECUTING, CHECK_END, FINISHED
};

struct AnimatedMonster {
    Monster monster;
    sf::Vector2f currentPos;
    sf::Vector2f targetPos;
    float moveTime;     
    float moveDuration; 

    bool isHit;
    float hitTimer;
};

class Battle {
public:
    Battle();
    void loadStage(int idx);
    void loadBoss();
    void setAllies(const std::vector<Monster>& a);
    void startRound();
    void playerSelectDie(int index);
    void update(float dt);
    void render(sf::RenderWindow& win);

    bool isFinished() const;
    bool playerWon() const;
    BattleState getState() const;

private:
    sf::Font font;
    sf::Text infoText;
    sf::Texture backgroundTex;
    sf::Sprite backgroundSprite;

    std::vector<Monster> allies;
    std::vector<Monster> enemies;
    
    std::vector<AnimatedMonster> animatedAllies;
    std::vector<AnimatedMonster> animatedEnemies;

    std::vector<DieInfo> playerDice;
    std::vector<DieInfo> enemyDice;

    BattleState state;
    float actionTimer; 
    bool winFlag;
    bool isAllyTurn; 

    void executeDieEffect(DieInfo& die, bool isPlayerAction);
    void renderDiceGroups(sf::RenderWindow& win);
    void calculateFormationPositions(bool isPlayerTeam);
    void rotateTeam(bool isPlayerTeam, bool left);
};