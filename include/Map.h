#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <map> // Thêm thư viện map
#include "Node.h"

class Map {
public:
    Map();
    void generateDefaultLayout();
    void render(sf::RenderWindow& win, sf::Font& font);
    bool clickNode(const sf::Vector2f& mousePos, int &outId);
    
    void movePlayerToNode(int id);
    Node* getNodeById(int id);
    std::vector<Node>& getNodes() { return nodes; }
    int getCurrentNodeId() const;

private:
    std::vector<Node> nodes;
    int currentNodeId;

    // Thêm kho chứa Texture để load ảnh map
    std::map<NodeType, sf::Texture> nodeTextures;
    void loadTextures(); // Hàm load ảnh
};