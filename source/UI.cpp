#include "UI.h"
#include <iostream>

using namespace std;

UI::UI() {}

bool UI::loadFont(const string& path) {
    if (!font.loadFromFile(path)) {
        cerr << "UI Error: Could not load font " << path << endl;
        return false;
    }
    return true;
}

void UI::drawButton(sf::RenderWindow& win, const string& text, const sf::FloatRect& rect, bool enabled) {
    sf::RectangleShape box({rect.width, rect.height});
    box.setPosition(rect.left, rect.top);
    box.setFillColor(enabled ? sf::Color(100, 180, 250) : sf::Color(100, 100, 100));
    box.setOutlineThickness(2);
    box.setOutlineColor(sf::Color::Black);
    win.draw(box);
    
    sf::Text t(text, font, 20);
    t.setFillColor(sf::Color::Black);
    // Canh giữa text (đơn giản)
    t.setPosition(rect.left + 10, rect.top + 5);
    win.draw(t);
}

void UI::drawHealthBar(sf::RenderWindow& win, float x, float y, float w, float h, int cur, int max) {
    // Background
    sf::RectangleShape bg({w, h});
    bg.setPosition(x, y);
    bg.setFillColor(sf::Color(50, 50, 50));
    win.draw(bg);
    
    // Foreground
    if (max > 0) {
        float pct = (float)cur / max;
        if (pct < 0) pct = 0; if (pct > 1) pct = 1;
        
        sf::RectangleShape fg({w * pct, h});
        fg.setPosition(x, y);
        fg.setFillColor(sf::Color::Green);
        win.draw(fg);
    }
}

void UI::drawSpriteCentered(sf::RenderWindow& win, const sf::Sprite& sp, float x, float y) {
    sf::Sprite temp = sp;
    sf::FloatRect b = temp.getLocalBounds();
    temp.setOrigin(b.width/2, b.height/2);
    temp.setPosition(x, y);
    win.draw(temp);
}