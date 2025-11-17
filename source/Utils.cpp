#include "Utils.h"
#include <ctime>
#include <random> 

using namespace std;

namespace Utils {
    // Biến sinh số ngẫu nhiên tĩnh
    static std::mt19937 rng((unsigned)time(nullptr));

    int randomInt(int a, int b) {
        std::uniform_int_distribution<int> dist(a, b);
        return dist(rng);
    }
    
    float randomFloat(float a, float b) {
        std::uniform_real_distribution<float> dist(a, b);
        return dist(rng);
    }

    bool pointInCircle(const sf::Vector2f& c, float r, const sf::Vector2f& p) {
        float dx = p.x - c.x;
        float dy = p.y - c.y;
        return dx*dx + dy*dy <= r*r;
    }

    string toString(int v) {
        return std::to_string(v);
    }
}