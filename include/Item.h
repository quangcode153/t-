#pragma once
#include <string>
#include "Monster.h" // Forward declaration nếu cần, nhưng ở đây include luôn cho tiện

enum class ItemType {
    ATK_UP,
    HP_UP,
    SHIELD,
    EXTRA_ATTACK,
    FULL_HEAL,
    ADD_SLOT
};

class Item {
public:
    Item(ItemType t, int val);
    ItemType getType() const;
    std::string getName() const;
    int getValue() const;
    void apply(Monster& m) const;

private:
    ItemType type;
    std::string name;
    int value;
};