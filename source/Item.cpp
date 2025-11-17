#include "Item.h"

using namespace std;

Item::Item(ItemType t, int val) : type(t), value(val) {
    switch (type) {
        case ItemType::ATK_UP: name = "Attack Up"; break;
        case ItemType::HP_UP: name = "Max HP Up"; break;
        case ItemType::SHIELD: name = "Shield"; break;
        default: name = "Unknown Item"; break;
    }
}

ItemType Item::getType() const { return type; }
string Item::getName() const { return name; }
int Item::getValue() const { return value; }

void Item::apply(Monster& m) const {
    switch (type) {
        case ItemType::ATK_UP: m.buffAttack(value); break;
        case ItemType::HP_UP: m.increaseMaxHP(value); break;
        case ItemType::SHIELD: m.addShield(value); break;
        case ItemType::FULL_HEAL: m.healFull(); break;
        case ItemType::ADD_SLOT: m.addItemSlot(); break;
        default: break;
    }
}