#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include "Node.h"

class Map {
public:
    Map();
    void generateDefaultLayout();
    void render(sf::RenderWindow& win, sf::Font& font);
    bool clickNode(const sf::Vector2f& mousePos);
    
    void movePlayerToNode(int id);
    Node* getNodeById(int id);
    std::vector<Node>& getNodes() { return nodes; }
    int getCurrentNodeId() const;

    void unlockNextNode();
    int getSelectedNodeId() const { return selectedNodeId; }
    bool canEnterSelectedNode() const;

private:
    std::vector<Node> nodes;
    int currentNodeId; 
    int maxUnlockedNodeId; 
    int selectedNodeId;

    std::map<NodeType, sf::Texture> nodeTextures;
    
    // --- MỚI: BIẾN BACKGROUND MAP ---
    sf::Texture mapBgTex;
    sf::Sprite mapBgSprite;
    // --------------------------------
    
    void loadTextures();
};