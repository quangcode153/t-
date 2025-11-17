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

    if (defDuration > 0) {
        dmg -= 2;
        if (dmg < 0) dmg = 0;
    }

    if (shield > 0) {
        int absorb = std::min(shield, dmg);
        shield -= absorb;
        dmg -= absorb;
    }

    if (dmg > 0) {
        currentHP -= dmg;
        if (currentHP < 0) currentHP = 0;
    }
}

bool Monster::tryDefend() {
    if (defCooldown > 0) return false;
    defDuration = 2; 
    defCooldown = 3; 
    return true;
}

void Monster::endTurn() {
    if (defDuration > 0) defDuration--;
    if (defCooldown > 0) defCooldown--;
}

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
void Monster::resetRoundStats() {
    shield = 0;       // Xóa giáp
    defDuration = 0;  // Xóa trạng thái giảm dame
    defCooldown = 0;  // Xóa hồi chiêu (nếu có)
}