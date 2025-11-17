#include "Monster.h"
#include <algorithm> 
#include <iostream>

using namespace std;

Monster::Monster(const string& n, int hp, int atk, const string& sprite)
    : name(n), maxHP(hp), currentHP(hp), attack(atk), shield(0), 
      itemSlots(0), spritePath(sprite), 
      defDuration(0), defCooldown(0)
{}

void Monster::takeDamage(int dmg) {
    if (dmg <= 0) return;

    // 1. Kiểm tra trạng thái Thủ (Defense)
    if (defDuration > 0) {
        dmg -= 2; // Giảm 2 dame
        if (dmg < 0) dmg = 0;
    }

    // 2. Trừ vào Giáp (Shield)
    if (shield > 0 && dmg > 0) {
        int absorb = std::min(shield, dmg);
        shield -= absorb;
        dmg -= absorb;
    }

    // 3. Trừ Máu (HP)
    if (dmg > 0) {
        currentHP -= dmg;
        if (currentHP < 0) currentHP = 0;
    }
}

bool Monster::tryDefend() {
    if (defCooldown > 0) return false;
    defDuration = 2; // Tác dụng 2 lượt
    defCooldown = 3; // Hồi chiêu 3 lượt
    return true;
}

void Monster::endTurn() {
    // Giảm thời gian hiệu ứng mỗi cuối lượt
    if (defDuration > 0) defDuration--;
    if (defCooldown > 0) defCooldown--;
}

// Reset chỉ số khi sang vòng mới (Fix lỗi bất tử)
void Monster::resetRoundStats() {
    shield = 0;       // Xóa Giáp
    defDuration = 0;  // Xóa luôn trạng thái Thủ
    // defCooldown = 0; // (Tùy chọn: giữ dòng này comment nếu muốn giữ hồi chiêu qua vòng)
}

// Các hàm Getter/Setter
void Monster::healFull() { currentHP = maxHP; }
void Monster::addShield(int val) { if (val > 0) shield += val; }
void Monster::buffAttack(int v) { attack += v; }
void Monster::increaseMaxHP(int v) { maxHP += v; currentHP += v; }
void Monster::addItemSlot() { itemSlots++; }

bool Monster::isAlive() const { return currentHP > 0; }
string Monster::getName() const { return name; }
int Monster::getHP() const { return currentHP; }
int Monster::getMaxHP() const { return maxHP; }
int Monster::getATK() const { return attack; }
int Monster::getShield() const { return shield; }
int Monster::getItemSlots() const { return itemSlots; }
string Monster::getSpritePath() const { return spritePath; }

// [QUAN TRỌNG] Đã xóa getDefDuration và getDefCooldown ở đây 
// vì chúng đã có trong file Monster.h