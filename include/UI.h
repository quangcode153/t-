#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class UI {
public:
    UI();
    bool loadFont(const std::string& path);
    void drawButton(sf::RenderWindow& win, const std::string& text, const sf::FloatRect& rect, bool enabled);
    void drawHealthBar(sf::RenderWindow& win, float x, float y, float w, float h, int cur, int max);
    void drawSpriteCentered(sf::RenderWindow& win, const sf::Sprite& sp, float x, float y);

private:
    sf::Font font;
};