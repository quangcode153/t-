#pragma once
#include <string>

class Monster {
public:
    Monster(const std::string& n, int hp, int atk, const std::string& sprite);

    void takeDamage(int dmg);
    bool tryDefend();
    void endTurn();
    void resetRoundStats(); // Reset chỉ số đầu vòng

    void healFull();
    void addShield(int val);
    void buffAttack(int v); 
    void increaseMaxHP(int v);
    void addItemSlot();

    bool isAlive() const;
    std::string getName() const;
    int getHP() const;
    int getMaxHP() const;
    
    // Hàm này sẽ trả về (Công Gốc + Buff)
    int getATK() const; 
    
    int getShield() const;
    int getItemSlots() const;
    std::string getSpritePath() const;
    int getDefDuration() const { return defDuration; }
    int getDefCooldown() const { return defCooldown; }
    int getBuffDuration() const { return buffDuration; } // Kiểm tra thời gian buff

private:
    std::string name;
    int maxHP;
    int currentHP;
    int attack;      // Công gốc
    int tempAttack;  // Công được buff thêm (Biến mới)
    int buffDuration;// Thời gian buff còn lại (Biến mới)

    int shield;
    int itemSlots;
    std::string spritePath;

    int defDuration;
    int defCooldown;
};