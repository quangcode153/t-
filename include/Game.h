#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Map.h"
#include "Battle.h"
#include "UI.h"
#include "Monster.h"

enum class GameState {
    MENU,
    MAP,
    BATTLE,
    RESULT
};

class Game {
public:
    Game();
    void run();

private:
    void handleEvent(sf::Event& e);
    void update(float dt);
    void render();

    sf::RenderWindow window;
    sf::Font font;
    
    GameState state;
    
    // Menu Background
    sf::Texture menuBackgroundTex;
    sf::Sprite menuBackgroundSprite;
    
    // --- MỚI: Biến cho Nút Start ---
    sf::RectangleShape startButton; // Hình dáng nút
    sf::Text startButtonText;       // Chữ trên nút
    // ------------------------------
    
    // Sub-systems
    Map map;
    Battle battle;
    UI ui;

    // Global data
    std::vector<Monster> allies;
};