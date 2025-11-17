#include "Monster.h"
#include <algorithm>
#include <iostream> // Debug nếu cần

using namespace std;

Monster::Monster(const string& n, int hp, int atk, const string& sprite)
    : name(n), maxHP(hp), currentHP(hp), attack(atk), shield(0), 
      itemSlots(0), spritePath(sprite), 
      defDuration(0), defCooldown(0) // Khởi tạo bằng 0
{}

void Monster::takeDamage(int dmg) {
    if (dmg <= 0) return;

    // --- MỚI: Nếu đang thủ, giảm 2 sát thương ---
    if (defDuration > 0) {
        dmg -= 2;
        if (dmg < 0) dmg = 0;
    }
    // -------------------------------------------

    // Trừ vào khiên trước
    if (shield > 0) {
        int absorb = std::min(shield, dmg);
        shield -= absorb;
        dmg -= absorb;
    }
    // Trừ vào máu
    if (dmg > 0) {
        currentHP -= dmg;
        if (currentHP < 0) currentHP = 0;
    }
}

// --- MỚI: Hàm kích hoạt thủ ---
bool Monster::tryDefend() {
    if (defCooldown > 0) return false; // Đang hồi chiêu, không dùng được
    
    defDuration = 2; // Tồn tại 2 lượt
    defCooldown = 3; // Hồi chiêu 3 lượt
    return true;
}

// --- MỚI: Gọi cuối mỗi lượt để cập nhật trạng thái ---
void Monster::endTurn() {
    if (defDuration > 0) defDuration--;
    if (defCooldown > 0) defCooldown--;
}

// ... (Các hàm get/set cũ giữ nguyên bên dưới) ...
void Monster::addShield(int val) { if (val > 0) shield += val; }
void Monster::buffAttack(int v) { attack += v; }
void Monster::increaseMaxHP(int v) { maxHP += v; currentHP += v; }
void Monster::healFull() { currentHP = maxHP; }
void Monster::addItemSlot() { itemSlots++; }
bool Monster::isAlive() const { return currentHP > 0; }
string Monster::getName() const { return name; }
int Monster::getHP() const { return currentHP; }
int Monster::getMaxHP() const { return maxHP; }
int Monster::getATK() const { return attack; }
int Monster::getShield() const { return shield; }
int Monster::getItemSlots() const { return itemSlots; }
string Monster::getSpritePath() const { return spritePath; }