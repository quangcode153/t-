#include "Battle.h"
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
    if (!font.loadFromFile("assets/fonts/arial.ttf")) cerr << "Err font\n";
    if (backgroundTex.loadFromFile("assets/sprites/Background_Battle.jpg")) {
        backgroundSprite.setTexture(backgroundTex);
    }

    infoText.setFont(font);
    infoText.setCharacterSize(24);
    infoText.setFillColor(sf::Color::White);
    infoText.setOutlineColor(sf::Color::Black);
    infoText.setOutlineThickness(2.f);
    infoText.setPosition(300.f, 20.f);
}

void Battle::loadStage(int idx) {
    if (idx >= 0 && idx < (int)BattleConfig::stages.size()) enemies = BattleConfig::stages[idx].enemies;
    else enemies.clear();
}
void Battle::loadBoss() { enemies = BattleConfig::boss.enemies; }
void Battle::setAllies(const vector<Monster>& a) { 
    allies = a; 
    animatedAllies.clear();
    for(const auto& m : allies) animatedAllies.push_back({m, {0,0}, {0,0}, 0.f, 0.f}); 
    calculateFormationPositions(true);
}

void Battle::calculateFormationPositions(bool isPlayerTeam) {
    vector<AnimatedMonster>& team = isPlayerTeam ? animatedAllies : animatedEnemies;
    const sf::Vector2f baseCenter = isPlayerTeam ? sf::Vector2f(320, 450) : sf::Vector2f(960, 450);
    const float baseRadius = 120.f;
    const float angularSpread = M_PI * 0.8f;
    
    vector<AnimatedMonster*> aliveMonsters;
    for(auto& am : team) if (am.monster.isAlive()) aliveMonsters.push_back(&am); // Lấy địa chỉ

    if (aliveMonsters.empty()) return;
    float angleStep = (aliveMonsters.size() > 1) ? angularSpread / (aliveMonsters.size() - 1) : 0;

    for (size_t i = 0; i < aliveMonsters.size(); ++i) {
        AnimatedMonster* am = aliveMonsters[i]; // Con trỏ
        float angle = -angularSpread / 2.f + i * angleStep; 
        sf::Vector2f targetPos = { baseCenter.x + baseRadius * cos(angle), baseCenter.y + baseRadius * sin(angle) * 0.8f };
        if (am->currentPos.x == 0 && am->currentPos.y == 0) am->currentPos = targetPos;
        am->targetPos = targetPos;
        am->moveTime = 0.f; am->moveDuration = 0.f;
    }
    
    // SỬA LỖI Ở ĐÂY: Dùng dấu chấm (.) vì am là tham chiếu, không phải con trỏ
    for(auto& am : team) {
        if (!am.monster.isAlive()) {
            am.targetPos = {-1000.f, -1000.f};
            am.currentPos = {-1000.f, -1000.f};
        }
    }
}
// --- CƠ CHẾ MỚI: RANDOM 4 CHIÊU ---
void Battle::startRound() {
    state = BattleState::WAIT_INPUT;
    dicePool.clear();
    isAllyTurn = true; // Phe ta luôn được ưu tiên đi lượt đầu của round mới

    // Tạo 4 xúc xắc ngẫu nhiên
    for (int i = 0; i < 4; ++i) {
        DieInfo die;
        die.isUsed = false;
        
        int typeRoll = Utils::randomInt(0, 100);
        if (typeRoll < 40) { // 40% Tấn công
            die.type = DiceFace::Attack;
            die.value = Utils::randomInt(3, 8); // Damage ngẫu nhiên 3-8
            die.description = "ATK " + Utils::toString(die.value);
        } else if (typeRoll < 70) { // 30% Thủ
            die.type = DiceFace::Shield;
            die.value = Utils::randomInt(2, 6); // Shield ngẫu nhiên 2-6
            die.description = "SHIELD " + Utils::toString(die.value);
        } else if (typeRoll < 85) { // 15% Buff ATK
            die.type = DiceFace::BuffATK;
            die.value = Utils::randomInt(1, 3); // Buff 1-3
            die.description = "BUFF " + Utils::toString(die.value);
        } else { // 15% Xoay tua
            die.type = DiceFace::RotateLeft; // Tạm mặc định xoay trái
            die.value = 0;
            die.description = "ROTATE";
        }
        dicePool.push_back(die);
    }

    // Reset animation enemies
    animatedEnemies.clear();
    for(const auto& m : enemies) animatedEnemies.push_back({m, {0,0}, {0,0}, 0.f, 0.f});
    
    calculateFormationPositions(true);
    calculateFormationPositions(false);
    
    infoText.setString("NEW ROUND! Use Dice [1] [2] [3] [4]");
}

// --- NGƯỜI CHƠI CHỌN XÚC XẮC ---
void Battle::playerSelectDie(int index) {
    if (state != BattleState::WAIT_INPUT) return;
    if (index < 0 || index >= (int)dicePool.size()) return;
    if (dicePool[index].isUsed) return; // Đã dùng rồi thì thôi

    // Thực hiện chiêu thức
    executeDieEffect(dicePool[index]);

    // Đánh dấu đã dùng
    dicePool[index].isUsed = true;

    // Đổi lượt (Nếu phe ta vừa đánh -> phe địch, và ngược lại)
    isAllyTurn = !isAllyTurn;

    // Chuyển trạng thái sang animation
    state = BattleState::EXECUTING;
    actionTimer = 1.0f; // Chờ 1s animation
}

void Battle::executeDieEffect(const DieInfo& die) {
    // Xác định phe Tấn Công (Actor) và phe Bị Đánh (Target)
    vector<Monster>& actorTeam = isAllyTurn ? allies : enemies;
    vector<Monster>& targetTeam = isAllyTurn ? enemies : allies;
    
    vector<AnimatedMonster>& animActor = isAllyTurn ? animatedAllies : animatedEnemies;

    // Kiểm tra con đầu đàn còn sống không
    if (!actorTeam[0].isAlive()) return; 

    string actorName = animActor[0].monster.getName();
    int targetIdx = findFirstAlive(targetTeam);

    infoText.setString((isAllyTurn ? "[ALLY] " : "[ENEMY] ") + actorName + " uses " + die.description);

    switch (die.type) {
        case DiceFace::Attack:
            if (targetIdx != -1) {
                // Dmg = Gốc + Dice Value
                int totalDmg = actorTeam[0].getATK() + die.value;
                targetTeam[targetIdx].takeDamage(totalDmg);
            }
            break;
        case DiceFace::Shield:
            actorTeam[0].addShield(die.value);
            break;
        case DiceFace::BuffATK:
            actorTeam[0].buffAttack(die.value);
            break;
        case DiceFace::RotateLeft:
            rotateTeam(isAllyTurn, true);
            break;
        default: break;
    }
}

void Battle::update(float dt) {
    // Animation lerp (Giữ nguyên)
    auto updateAnim = [&](vector<AnimatedMonster>& list) {
        for (auto& am : list) {
            if (am.moveTime > 0) {
                float t = 1.f - (am.moveTime / am.moveDuration);
                am.currentPos = am.currentPos * (1.f - t) + am.targetPos * t; // Simple lerp
                am.moveTime -= dt;
                if (am.moveTime <= 0) am.currentPos = am.targetPos;
            } else am.currentPos = am.targetPos;
        }
    };
    updateAnim(animatedAllies);
    updateAnim(animatedEnemies);

    if (state == BattleState::EXECUTING) {
        if (actionTimer > 0) { actionTimer -= dt; return; }
        
        // Hết thời gian animation -> Check kết thúc
        state = BattleState::CHECK_END;
    }
    else if (state == BattleState::CHECK_END) {
        // Đồng bộ data
        for(size_t i=0; i<allies.size(); ++i) animatedAllies[i].monster = allies[i];
        for(size_t i=0; i<enemies.size(); ++i) animatedEnemies[i].monster = enemies[i];
        calculateFormationPositions(true);
        calculateFormationPositions(false);

        int aAlive = 0; for(auto& m : allies) if(m.isAlive()) aAlive++;
        int eAlive = 0; for(auto& m : enemies) if(m.isAlive()) eAlive++;

        if (aAlive == 0) { winFlag = false; state = BattleState::FINISHED; }
        else if (eAlive == 0) { winFlag = true; state = BattleState::FINISHED; }
        else {
            // Kiểm tra xem còn xúc xắc nào chưa dùng không
            bool anyDiceLeft = false;
            for(const auto& d : dicePool) if(!d.isUsed) anyDiceLeft = true;

            if (anyDiceLeft) {
                state = BattleState::WAIT_INPUT; // Tiếp tục chọn
                infoText.setString(isAllyTurn ? "YOUR TURN: Pick a Dice" : "ENEMY TURN: Pick a Dice for Enemy");
            } else {
                // Hết xúc xắc -> Hết vòng -> Random lại
                for(auto& m : allies) m.endTurn(); // Giảm cooldown (nếu có)
                for(auto& m : enemies) m.endTurn();
                startRound();
            }
        }
    }
}

void Battle::renderDicePool(sf::RenderWindow& win) {
    // Vẽ 4 ô xúc xắc ở giữa dưới màn hình
    float startX = 440.f;
    float startY = 600.f;
    float gap = 110.f;

    for (int i = 0; i < (int)dicePool.size(); ++i) {
        sf::RectangleShape box(sf::Vector2f(100.f, 100.f));
        box.setPosition(startX + i * gap, startY);
        box.setOutlineThickness(3.f);
        box.setOutlineColor(sf::Color::Black);

        if (dicePool[i].isUsed) {
            box.setFillColor(sf::Color(100, 100, 100, 150)); // Xám nếu đã dùng
        } else {
            // Màu sắc tùy loại
            switch(dicePool[i].type) {
                case DiceFace::Attack: box.setFillColor(sf::Color(255, 100, 100)); break; // Đỏ
                case DiceFace::Shield: box.setFillColor(sf::Color(100, 100, 255)); break; // Xanh
                case DiceFace::BuffATK: box.setFillColor(sf::Color(255, 255, 100)); break; // Vàng
                default: box.setFillColor(sf::Color::White); break;
            }
        }
        win.draw(box);

        // Vẽ text mô tả
        sf::Text t(dicePool[i].description, font, 18);
        t.setFillColor(sf::Color::Black);
        sf::FloatRect b = t.getLocalBounds();
        t.setOrigin(b.width/2, b.height/2);
        t.setPosition(box.getPosition().x + 50.f, box.getPosition().y + 50.f);
        win.draw(t);

        // Vẽ số phím tắt [1] [2]...
        sf::Text num("[" + Utils::toString(i+1) + "]", font, 16);
        num.setFillColor(sf::Color::White);
        num.setOutlineColor(sf::Color::Black);
        num.setOutlineThickness(1.f);
        num.setPosition(box.getPosition().x + 5, box.getPosition().y + 5);
        win.draw(num);
    }
}

void Battle::render(sf::RenderWindow& win) {
    win.draw(backgroundSprite);

    // ... (Giữ nguyên phần vẽ Sprite Character như cũ) ...
    // Code vẽ character ở đây dài dòng nên mình không paste lại, bạn giữ nguyên phần đó
    // Chỉ cần đảm bảo paste phần vẽ DicePool vào cuối

    // Copy lại phần vẽ Allies/Enemies từ code cũ vào đây
    // ...
    // VẼ ALLIES
    for (int i = 0; i < (int)animatedAllies.size(); ++i) {
        if (!animatedAllies[i].monster.isAlive()) continue;
        string path = animatedAllies[i].monster.getSpritePath();
        if (textureCache.find(path) == textureCache.end()) textureCache[path].loadFromFile(path);
        sf::Sprite sp(textureCache[path]);
        sf::FloatRect bounds = sp.getLocalBounds();
        sp.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
        sp.setScale(0.35f, 0.35f); 
        sp.setPosition(animatedAllies[i].currentPos);
        if (i == 0) sp.setColor(sf::Color::White); else sp.setColor(sf::Color(150, 150, 150));
        win.draw(sp);
        sf::Text t(animatedAllies[i].monster.getName() + "\nHP: " + Utils::toString(animatedAllies[i].monster.getHP()), font, 16);
        t.setOutlineColor(sf::Color::Black); t.setOutlineThickness(1.5f); t.setOrigin(t.getLocalBounds().width/2, 0);
        t.setPosition(animatedAllies[i].currentPos.x, animatedAllies[i].currentPos.y - 100);
        if(i==0) t.setFillColor(sf::Color::Cyan);
        win.draw(t);
    }
    // VẼ ENEMIES
    for (int i = 0; i < (int)animatedEnemies.size(); ++i) {
        if (!animatedEnemies[i].monster.isAlive()) continue;
        string path = animatedEnemies[i].monster.getSpritePath();
        if (textureCache.find(path) == textureCache.end()) textureCache[path].loadFromFile(path);
        sf::Sprite sp(textureCache[path]);
        sf::FloatRect bounds = sp.getLocalBounds();
        sp.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
        sp.setScale(-0.35f, 0.35f); 
        sp.setPosition(animatedEnemies[i].currentPos);
        if (i == 0) sp.setColor(sf::Color::White); else sp.setColor(sf::Color(150, 150, 150));
        win.draw(sp);
        sf::Text t("HP: " + Utils::toString(animatedEnemies[i].monster.getHP()), font, 16);
        t.setFillColor(sf::Color(255, 100, 100)); t.setOutlineColor(sf::Color::Black); t.setOutlineThickness(1.5f);
        t.setOrigin(t.getLocalBounds().width/2, 0);
        t.setPosition(animatedEnemies[i].currentPos.x, animatedEnemies[i].currentPos.y - 100);
        win.draw(t);
    }

    // MỚI: Vẽ Info Text và Bảng Xúc Xắc
    win.draw(infoText);
    renderDicePool(win);
}

bool Battle::isFinished() const { return state == BattleState::FINISHED; }
bool Battle::playerWon() const { return winFlag; }
BattleState Battle::getState() const { return state; }

// Giữ nguyên hàm rotate
void Battle::rotateTeam(bool isPlayerTeam, bool left) {
    vector<Monster> &team = isPlayerTeam ? allies : enemies;
    vector<AnimatedMonster> &animatedTeam = isPlayerTeam ? animatedAllies : animatedEnemies;
    if (team.empty()) return;

    if (left) {
        Monster firstMonster = team[0];
        for (size_t i = 0; i + 1 < team.size(); ++i) team[i] = team[i+1];
        team[team.size()-1] = firstMonster;
    }
    for (size_t i = 0; i < team.size(); ++i) {
        animatedTeam[i].monster = team[i];
        animatedTeam[i].currentPos = animatedTeam[i].targetPos; 
        animatedTeam[i].moveTime = 0.2f; 
        animatedTeam[i].moveDuration = 0.2f;
    }
    calculateFormationPositions(isPlayerTeam);
}
