#include "Battle.h"
#include "BattleConfig.h"
#include "Utils.h"
#include <iostream>
#include <algorithm>
#include <map>
#include <cmath> 

using namespace std;

static int findFirstAlive(const vector<Monster>& team) {
    for (int i = 0; i < (int)team.size(); ++i) if (team[i].isAlive()) return i;
    return -1;
}

static std::map<string, sf::Texture> textureCache;

Battle::Battle()
    : actionTimer(0.f), state(BattleState::START_ROUND), winFlag(false), isAllyTurn(true)
{
    if (!font.loadFromFile("assets/fonts/arial.ttf")) cerr << "Error loading font\n";
    if (backgroundTex.loadFromFile("assets/sprites/Background_Battle.jpg")) {
        backgroundSprite.setTexture(backgroundTex);
    }
    infoText.setFont(font);
    infoText.setCharacterSize(24);
    infoText.setFillColor(sf::Color::White);
    infoText.setOutlineColor(sf::Color::Black);
    infoText.setOutlineThickness(2.f);
    infoText.setPosition(350.f, 30.f);
}

void Battle::loadStage(int idx) {
    // Kiểm tra an toàn để tránh crash nếu config lỗi
    if (idx >= 0 && idx < (int)BattleConfig::stages.size()) {
        enemies = BattleConfig::stages[idx].enemies;
    } else {
        enemies.clear();
    }
}

void Battle::loadBoss() { enemies = BattleConfig::boss.enemies; }

void Battle::setAllies(const vector<Monster>& a) { 
    allies = a; 
    // Khởi tạo animatedAllies ngay khi set
    animatedAllies.clear();
    for(const auto& m : allies) {
        animatedAllies.push_back({m, {0,0}, {0,0}, 0.f, 0.f, false, 0.f}); 
    }
    calculateFormationPositions(true);
}

// --- [FIX QUAN TRỌNG] SỬA LẠI LOGIC START ROUND ---
void Battle::startRound() {
    state = BattleState::WAIT_INPUT;
    playerDice.clear();
    enemyDice.clear();
    isAllyTurn = true; 

    // 1. Reset chỉ số (Giáp/Thủ) của dữ liệu gốc
    for(auto& m : allies) m.resetRoundStats();
    for(auto& m : enemies) m.resetRoundStats();
    
    // 2. [FIX] Khởi tạo lại AnimatedEnemies NGAY TẠI ĐÂY
    // Để đảm bảo kích thước mảng khớp với số lượng quái vật hiện tại
    animatedEnemies.clear();
    for(const auto& m : enemies) {
        animatedEnemies.push_back({m, {0,0}, {0,0}, 0.f, 0.f, false, 0.f});
    }

    // 3. Đồng bộ lại AnimatedAllies (để cập nhật việc mất giáp trên UI)
    for(size_t i=0; i<allies.size(); ++i) {
        if(i < animatedAllies.size()) animatedAllies[i].monster = allies[i];
    }

    // 4. Random Xúc Xắc
    for (int i = 0; i < 2; ++i) {
        DieInfo die;
        die.isUsed = false;
        int typeRoll = Utils::randomInt(0, 100);
        
        if (typeRoll < 40) { die.type = DiceFace::Attack; die.value = Utils::randomInt(3, 8); die.description = "ATK"; } 
        else if (typeRoll < 70) { die.type = DiceFace::Shield; die.value = Utils::randomInt(2, 6); die.description = "DEF"; } 
        else if (typeRoll < 85) { die.type = DiceFace::BuffATK; die.value = Utils::randomInt(1, 3); die.description = "BUFF"; } 
        else { die.type = DiceFace::RotateLeft; die.value = 0; die.description = "ROT"; }
        
        playerDice.push_back(die);
        enemyDice.push_back(die);
    }

    // 5. Tính toán vị trí đứng
    calculateFormationPositions(true);
    calculateFormationPositions(false);
    
    infoText.setString("LEFT TURN: Use [1] or [2]");
}

void Battle::calculateFormationPositions(bool isPlayerTeam) {
    vector<AnimatedMonster>& team = isPlayerTeam ? animatedAllies : animatedEnemies;
    const sf::Vector2f baseCenter = isPlayerTeam ? sf::Vector2f(320, 450) : sf::Vector2f(960, 450);
    const float baseRadius = 120.f;
    const float angularSpread = M_PI * 0.8f;
    
    vector<AnimatedMonster*> aliveMonsters;
    for(auto& am : team) if (am.monster.isAlive()) aliveMonsters.push_back(&am);

    if (aliveMonsters.empty()) return;

    float angleStep = (aliveMonsters.size() > 1) ? angularSpread / (aliveMonsters.size() - 1) : 0;

    for (size_t i = 0; i < aliveMonsters.size(); ++i) {
        AnimatedMonster* am = aliveMonsters[i];
        float angle = -angularSpread / 2.f + i * angleStep; 
        sf::Vector2f targetPos = { baseCenter.x + baseRadius * cos(angle), baseCenter.y + baseRadius * sin(angle) * 0.8f };
        if (am->currentPos.x == 0 && am->currentPos.y == 0) am->currentPos = targetPos;
        am->targetPos = targetPos;
        am->moveTime = 0.f; am->moveDuration = 0.f;
    }
    for(auto& am : team) if (!am.monster.isAlive()) { am.targetPos = {-1000.f, -1000.f}; am.currentPos = {-1000.f, -1000.f}; }
}

void Battle::playerSelectDie(int index) {
    if (state != BattleState::WAIT_INPUT) return;

    if (isAllyTurn) {
        if (index == 2 || index == 3) return; 
        if (index < 0 || index >= (int)playerDice.size()) return;
        if (playerDice[index].isUsed) return;

        executeDieEffect(playerDice[index], true);
        playerDice[index].isUsed = true;
        isAllyTurn = false; 
        infoText.setString("RIGHT TURN: Use [3] or [4]");
    } else {
        if (index == 0 || index == 1) return;
        int realIndex = index - 2; 
        if (realIndex < 0 || realIndex >= (int)enemyDice.size()) return;
        if (enemyDice[realIndex].isUsed) return;

        executeDieEffect(enemyDice[realIndex], false);
        enemyDice[realIndex].isUsed = true;
        isAllyTurn = true; 
        infoText.setString("LEFT TURN: Use [1] or [2]");
    }
    state = BattleState::EXECUTING;
    actionTimer = 1.0f;
}

void Battle::executeDieEffect(DieInfo& die, bool isPlayerAction) {
    vector<Monster>* pActorTeam = isPlayerAction ? &allies : &enemies;
    vector<Monster>* pTargetTeam = isPlayerAction ? &enemies : &allies;
    vector<AnimatedMonster>* pAnimActor = isPlayerAction ? &animatedAllies : &animatedEnemies;
    vector<AnimatedMonster>* pAnimTarget = isPlayerAction ? &animatedEnemies : &animatedAllies;

    if (pActorTeam->empty() || !(*pActorTeam)[0].isAlive()) return; 

    string actorName = (*pAnimActor)[0].monster.getName();
    int targetIdx = findFirstAlive(*pTargetTeam);

    string actionDesc = "";
    if (die.type == DiceFace::Attack) actionDesc = "Attack " + Utils::toString(die.value);
    else if (die.type == DiceFace::Shield) actionDesc = "Shield " + Utils::toString(die.value);
    else if (die.type == DiceFace::BuffATK) actionDesc = "Buff ATK";
    else actionDesc = "Rotate";

    infoText.setString((isPlayerAction ? "[LEFT] " : "[RIGHT] ") + actorName + " uses " + actionDesc);

    vector<Monster>& targetTeamRef = *pTargetTeam;
    vector<Monster>& actorTeamRef = *pActorTeam;
    vector<AnimatedMonster>& animTargetRef = *pAnimTarget;
    vector<AnimatedMonster>& animActorRef = *pAnimActor;

    switch (die.type) {
        case DiceFace::Attack:
            if (targetIdx != -1) {
                targetTeamRef[targetIdx].takeDamage(die.value);
                animTargetRef[targetIdx].monster = targetTeamRef[targetIdx]; 
                if (animTargetRef[targetIdx].monster.isAlive()) {
                    animTargetRef[targetIdx].isHit = true;
                    animTargetRef[targetIdx].hitTimer = 0.4f; 
                }
            }
            break;
        case DiceFace::Shield:
            actorTeamRef[0].addShield(die.value);
            animActorRef[0].monster = actorTeamRef[0]; 
            break;
        case DiceFace::BuffATK:
            actorTeamRef[0].buffAttack(die.value);
            animActorRef[0].monster = actorTeamRef[0];
            break;
        case DiceFace::RotateLeft:
            rotateTeam(isPlayerAction, true);
            break;
        default: break;
    }
}

void Battle::update(float dt) {
    auto updateAnimAndHit = [&](vector<AnimatedMonster>& list) {
        for (auto& am : list) {
            if (am.moveTime > 0) {
                float t = 1.f - (am.moveTime / am.moveDuration);
                am.currentPos.x = am.currentPos.x * (1.f - t) + am.targetPos.x * t;
                am.currentPos.y = am.currentPos.y * (1.f - t) + am.targetPos.y * t;
                am.moveTime -= dt;
                if (am.moveTime <= 0) am.currentPos = am.targetPos;
            } else am.currentPos = am.targetPos;

            if (am.isHit) {
                am.hitTimer -= dt;
                if (am.hitTimer <= 0) am.isHit = false;
            }
        }
    };
    updateAnimAndHit(animatedAllies);
    updateAnimAndHit(animatedEnemies);

    if (state == BattleState::EXECUTING) {
        if (actionTimer > 0) { actionTimer -= dt; return; }
        state = BattleState::CHECK_END;
    }
    else if (state == BattleState::CHECK_END) {
        for(size_t i=0; i<allies.size(); ++i) animatedAllies[i].monster = allies[i];
        for(size_t i=0; i<enemies.size(); ++i) animatedEnemies[i].monster = enemies[i];
        calculateFormationPositions(true);
        calculateFormationPositions(false);

        int aAlive = 0; for(auto& m : allies) if(m.isAlive()) aAlive++;
        int eAlive = 0; for(auto& m : enemies) if(m.isAlive()) eAlive++;

        if (aAlive == 0) { winFlag = false; state = BattleState::FINISHED; }
        else if (eAlive == 0) { winFlag = true; state = BattleState::FINISHED; }
        else {
            bool leftHasDice = false; for(auto& d : playerDice) if(!d.isUsed) leftHasDice = true;
            bool rightHasDice = false; for(auto& d : enemyDice) if(!d.isUsed) rightHasDice = true;

            if (!leftHasDice && !rightHasDice) {
                for(auto& m : allies) m.endTurn();
                for(auto& m : enemies) m.endTurn();
                startRound();
            } else {
                state = BattleState::WAIT_INPUT;
            }
        }
    }
}

void Battle::renderDiceGroups(sf::RenderWindow& win) {
    float gap = 120.f;
    float yPos = 600.f; 

    auto drawDie = [&](float x, float y, DieInfo& die, sf::Color outlineCol, string keyHint) {
        sf::RectangleShape box(sf::Vector2f(100.f, 100.f));
        box.setPosition(x, y);
        box.setOutlineThickness(3.f);
        box.setOutlineColor(outlineCol);

        if (die.isUsed) {
            box.setFillColor(sf::Color(50, 50, 50, 200));
        } else {
            switch(die.type) {
                case DiceFace::Attack: box.setFillColor(sf::Color(220, 60, 60)); break; 
                case DiceFace::Shield: box.setFillColor(sf::Color(60, 100, 220)); break; 
                case DiceFace::BuffATK: box.setFillColor(sf::Color(220, 220, 60)); break; 
                default: box.setFillColor(sf::Color::White); break;
            }
        }
        win.draw(box);

        string mainText = "", subText = "";
        if (die.type == DiceFace::RotateLeft) { mainText = "ROT"; subText = "Move"; } 
        else {
            mainText = Utils::toString(die.value); 
            if (die.type == DiceFace::Attack) subText = "DMG";
            else if (die.type == DiceFace::Shield) subText = "DEF";
            else if (die.type == DiceFace::BuffATK) subText = "BUFF";
        }

        sf::Text tVal(mainText, font, 36);
        tVal.setStyle(sf::Text::Bold);
        tVal.setFillColor(die.isUsed ? sf::Color(150, 150, 150) : sf::Color::Black);
        sf::FloatRect b = tVal.getLocalBounds();
        tVal.setOrigin(b.left + b.width/2.0f, b.top + b.height/2.0f);
        tVal.setPosition(x + 50.f, y + 40.f);
        win.draw(tVal);

        sf::Text tSub(subText, font, 16);
        tSub.setFillColor(die.isUsed ? sf::Color(150, 150, 150) : sf::Color(20, 20, 20));
        sf::FloatRect b2 = tSub.getLocalBounds();
        tSub.setOrigin(b2.left + b2.width/2.0f, b2.top + b2.height/2.0f);
        tSub.setPosition(x + 50.f, y + 75.f);
        win.draw(tSub);

        if (!die.isUsed && !keyHint.empty()) {
            sf::Text k(keyHint, font, 14);
            k.setFillColor(sf::Color::White); k.setOutlineColor(sf::Color::Black); k.setOutlineThickness(1.5f);
            k.setPosition(x + 5, y + 2);
            win.draw(k);
        }
    };

    float leftX = 200.f; 
    for (int i = 0; i < (int)playerDice.size(); ++i) {
        sf::Color border = isAllyTurn ? sf::Color::Cyan : sf::Color(100, 100, 100);
        string hint = isAllyTurn ? "[" + Utils::toString(i+1) + "]" : "";
        drawDie(leftX + i * gap, yPos, playerDice[i], border, hint);
    }

    float rightX = 850.f; 
    for (int i = 0; i < (int)enemyDice.size(); ++i) {
        sf::Color border = !isAllyTurn ? sf::Color::Red : sf::Color(100, 100, 100);
        string hint = !isAllyTurn ? "[" + Utils::toString(i+3) + "]" : "";
        drawDie(rightX + i * gap, yPos, enemyDice[i], border, hint);
    }
}

void Battle::render(sf::RenderWindow& win) {
    win.draw(backgroundSprite);

    auto renderTeam = [&](vector<AnimatedMonster>& team, bool isLeft) {
        for (int i = 0; i < (int)team.size(); ++i) {
            if (!team[i].monster.isAlive()) continue;
            string path = team[i].monster.getSpritePath();
            if (textureCache.find(path) == textureCache.end()) textureCache[path].loadFromFile(path);
            
            sf::Sprite sp(textureCache[path]);
            sf::FloatRect bounds = sp.getLocalBounds();
            sp.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
            
            float scaleX = isLeft ? 0.35f : -0.35f;
            sp.setScale(scaleX, 0.35f); 
            sp.setPosition(team[i].currentPos);

            if (team[i].isHit) {
                if (fmod(team[i].hitTimer, 0.1f) < 0.05f) sp.setColor(sf::Color::Red); 
                else sp.setColor(sf::Color::White);
            } else {
                if (i == 0) sp.setColor(sf::Color::White); else sp.setColor(sf::Color(150, 150, 150));
            }
            win.draw(sp);

            string status = "";
            int sh = team[i].monster.getShield();
            if (sh > 0) status += " (Shld:" + Utils::toString(sh) + ")";
            
            sf::Text t(team[i].monster.getName() + "\nHP: " + Utils::toString(team[i].monster.getHP()) + status, font, 16);
            t.setOutlineColor(sf::Color::Black); t.setOutlineThickness(1.5f); t.setOrigin(t.getLocalBounds().width/2, 0);
            t.setPosition(team[i].currentPos.x, team[i].currentPos.y - 110);
            t.setFillColor((sh>0)?sf::Color::Yellow : sf::Color::White);
            win.draw(t);
        }
    };

    renderTeam(animatedAllies, true);
    renderTeam(animatedEnemies, false);

    win.draw(infoText);
    renderDiceGroups(win); 
}

bool Battle::isFinished() const { return state == BattleState::FINISHED; }
bool Battle::playerWon() const { return winFlag; }
BattleState Battle::getState() const { return state; }

void Battle::rotateTeam(bool isPlayerTeam, bool left) {
    vector<Monster> &team = isPlayerTeam ? allies : enemies;
    vector<AnimatedMonster> &animatedTeam = isPlayerTeam ? animatedAllies : animatedEnemies;
    if (team.empty()) return;

    if (left) {
        Monster firstMonster = team[0];
        for (size_t i = 0; i + 1 < team.size(); ++i) team[i] = team[i+1];
        team[team.size()-1] = firstMonster;
    } else {
        Monster lastMonster = team[team.size()-1];
        for (size_t i = team.size()-1; i > 0; --i) team[i] = team[i-1];
        team[0] = lastMonster;
    }
    for (size_t i = 0; i < team.size(); ++i) {
        animatedTeam[i].monster = team[i];
        animatedTeam[i].currentPos = animatedTeam[i].targetPos; 
        animatedTeam[i].moveTime = 0.2f; 
        animatedTeam[i].moveDuration = 0.2f;
    }
    calculateFormationPositions(isPlayerTeam);
}