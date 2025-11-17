#pragma once
#include <string>

class Monster {
public:
    Monster(const std::string& n, int hp, int atk, const std::string& sprite);

    // Sửa hàm takeDamage để tính cả thủ
    void takeDamage(int dmg);
    
    // Các hàm mới cho cơ chế thủ
    bool tryDefend(); // Trả về true nếu thủ thành công (không bị cooldown)
    void endTurn();   // Gọi cuối mỗi lượt để giảm cooldown/duration
    
    // Giữ nguyên các hàm cũ
    void healFull();
    void addShield(int val);
    void buffAttack(int v);
    void increaseMaxHP(int v);
    void addItemSlot();

    bool isAlive() const;
    std::string getName() const;
    int getHP() const;
    int getMaxHP() const;
    int getATK() const;
    int getShield() const;
    int getItemSlots() const;
    std::string getSpritePath() const;
    
    // Getter cho UI hiển thị
    int getDefDuration() const { return defDuration; }
    int getDefCooldown() const { return defCooldown; }

private:
    std::string name;
    int maxHP;
    int currentHP;
    int attack;
    int shield;
    int itemSlots;
    std::string spritePath;

    // --- MỚI: Biến cho cơ chế thủ ---
    int defDuration; // Số lượt còn tác dụng phòng thủ (giảm 2 dmg)
    int defCooldown; // Số lượt phải chờ để dùng lại
};