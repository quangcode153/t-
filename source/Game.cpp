#include "Game.h"
#include "BattleConfig.h"
#include <iostream>

using namespace std;

Game::Game() {
    window.create(sf::VideoMode(1280, 720), "Dicefolk Lite");
    window.setFramerateLimit(60);

    if (!font.loadFromFile("assets/fonts/arial.ttf")) {
        cerr << "Font load FAILED!\n";
    }
    ui.loadFont("assets/fonts/arial.ttf");

    // Load Menu Background
    if (!menuBackgroundTex.loadFromFile("assets/sprites/nenmenu.png")) {
        cerr << "Menu Error: Failed to load assets/sprites/nenmenu.png\n";
    } else {
        menuBackgroundSprite.setTexture(menuBackgroundTex);
        sf::Vector2u texSize = menuBackgroundTex.getSize();
        sf::Vector2u winSize = window.getSize();
        menuBackgroundSprite.setScale((float)winSize.x / texSize.x, (float)winSize.y / texSize.y);
    }

    // Setup Start Button
    startButton.setSize(sf::Vector2f(220.f, 60.f));
    startButton.setFillColor(sf::Color(0, 0, 0, 180));
    startButton.setOutlineThickness(3.f);
    startButton.setOutlineColor(sf::Color::White);
    startButton.setOrigin(110.f, 30.f);
    startButton.setPosition(640.f, 580.f);

    startButtonText.setFont(font);
    startButtonText.setString("START GAME");
    startButtonText.setCharacterSize(28);
    startButtonText.setFillColor(sf::Color::White);
    startButtonText.setStyle(sf::Text::Bold);
    sf::FloatRect tr = startButtonText.getLocalBounds();
    startButtonText.setOrigin(tr.left + tr.width/2.0f, tr.top + tr.height/2.0f);
    startButtonText.setPosition(startButton.getPosition());

    // Game Data
    BattleConfig::initStages();
    map.generateDefaultLayout();
    
    allies = {
        Monster("Hero", 20, 4, "assets/sprites/Akama.png"),      
        Monster("Guard", 15, 3, "assets/sprites/Beloid_A.png"),  
        Monster("Pet",   10, 2, "assets/sprites/Boglin.png")     
    };

    state = GameState::MENU;
}

void Game::run() {
    sf::Clock delta;
    while (window.isOpen()) {
        float dt = delta.restart().asSeconds();
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) window.close();
            handleEvent(e);
        }
        update(dt);
        render();
    }
}

void Game::handleEvent(sf::Event& e) {
    if (state == GameState::MENU) {
        if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            if (startButton.getGlobalBounds().contains(mousePos)) {
                state = GameState::MAP;
            }
        }
    }
    else if (state == GameState::MAP) {
        if (e.type == sf::Event::MouseButtonPressed) {
            sf::Vector2f mPos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            int id;
            if (map.clickNode(mPos, id)) {
                map.movePlayerToNode(id);
                Node* n = map.getNodeById(id);
                if (n && (n->type == NodeType::Battle || n->type == NodeType::Boss)) {
                    if (n->type == NodeType::Battle) battle.loadStage(n->stageID);
                    else battle.loadBoss();
                    battle.setAllies(allies);
                    battle.startRound();
                    state = GameState::BATTLE;
                }
            }
        }
    }
    else if (state == GameState::BATTLE) {
        if (e.type == sf::Event::KeyPressed) {
            // Phím 1 -> Chọn xúc xắc 0
            if (e.key.code == sf::Keyboard::Num1 || e.key.code == sf::Keyboard::Numpad1) {
                battle.playerSelectDie(0);
            }
            // Phím 2 -> Chọn xúc xắc 1
            if (e.key.code == sf::Keyboard::Num2 || e.key.code == sf::Keyboard::Numpad2) {
                battle.playerSelectDie(1);
            }
            // Phím 3 -> Chọn xúc xắc 2
            if (e.key.code == sf::Keyboard::Num3 || e.key.code == sf::Keyboard::Numpad3) {
                battle.playerSelectDie(2);
            }
            // Phím 4 -> Chọn xúc xắc 3
            if (e.key.code == sf::Keyboard::Num4 || e.key.code == sf::Keyboard::Numpad4) {
                battle.playerSelectDie(3);
            }

            if (e.key.code == sf::Keyboard::Enter && battle.isFinished()) {
                state = GameState::RESULT;
            }
        }
    }
    else if (state == GameState::RESULT) {
        if (e.type == sf::Event::MouseButtonPressed || e.type == sf::Event::KeyPressed) {
            state = GameState::MAP;
        }
    }
}

void Game::update(float dt) {
    if (state == GameState::BATTLE) {
        battle.update(dt);
    }
}

void Game::render() {
    window.clear(sf::Color(40,40,40)); 

    if (state == GameState::MENU) {
        window.draw(menuBackgroundSprite);
        
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        if (startButton.getGlobalBounds().contains(mousePos)) {
            startButton.setFillColor(sf::Color(50, 200, 50, 200)); 
            startButton.setScale(1.05f, 1.05f);
            startButtonText.setScale(1.05f, 1.05f);
        } else {
            startButton.setFillColor(sf::Color(0, 0, 0, 180));
            startButton.setScale(1.0f, 1.0f);
            startButtonText.setScale(1.0f, 1.0f);
        }
        window.draw(startButton);
        window.draw(startButtonText);
    }
    else if (state == GameState::MAP) {
        map.render(window, font);
    }
    else if (state == GameState::BATTLE) {
        battle.render(window);

        if (battle.isFinished()) {
            sf::Text t(battle.playerWon() ? "VICTORY! Press Enter" : "DEFEAT! Press Enter", font, 50);
            t.setFillColor(battle.playerWon() ? sf::Color::Green : sf::Color::Red);
            t.setOutlineColor(sf::Color::Black); t.setOutlineThickness(3.f);
            sf::FloatRect b = t.getLocalBounds();
            t.setOrigin(b.width/2, b.height/2);
            t.setPosition(640, 360);
            window.draw(t);
        }
    }
    else if (state == GameState::RESULT) {
        sf::Text t("Click to return Map...", font, 40);
        t.setOutlineColor(sf::Color::Black); t.setOutlineThickness(2.f);
        t.setPosition(400, 300);
        window.draw(t);
    }
    window.display();
}