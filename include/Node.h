#pragma once
#include <SFML/System/Vector2.hpp>
#include <vector>

enum class NodeType {
    Empty,      // Dùng ảnh Start hoặc Empty
    Battle,     // Dùng ảnh Battle.png
    Boss,       // Dùng ảnh Boss.png
    Bonfire,    // Dùng ảnh Bonfire.png (Hồi máu)
    Berry,      // Dùng ảnh Broodberries.png (Thức ăn/Treasure)
    Shop        // Dùng ảnh Abandoned_tent.png
};

struct Node {
    int id;
    sf::Vector2f pos;
    NodeType type;
    std::vector<int> neighbors;
    bool visited = false;
    int stageID = 0;
};