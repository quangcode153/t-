#pragma once
#include <SFML/System/Vector2.hpp>
#include <string>
#include <random>

namespace Utils {
    int randomInt(int a, int b);
    float randomFloat(float a, float b);
    bool pointInCircle(const sf::Vector2f& c, float r, const sf::Vector2f& p);
    std::string toString(int v);
}