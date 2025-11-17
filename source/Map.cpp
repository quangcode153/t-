#include "Map.h"
#include "Utils.h"
#include <iostream>

using namespace std;

Map::Map() : currentNodeId(-1) {
    loadTextures(); // Gọi hàm load ảnh ngay khi tạo Map
}

void Map::loadTextures() {
    // Load các icon tương ứng
    nodeTextures[NodeType::Battle].loadFromFile("assets/sprites/Battle.png");
    nodeTextures[NodeType::Boss].loadFromFile("assets/sprites/Boss.png");
    nodeTextures[NodeType::Bonfire].loadFromFile("assets/sprites/Bonfire.png");
    nodeTextures[NodeType::Berry].loadFromFile("assets/sprites/Broodberries.png");
    nodeTextures[NodeType::Shop].loadFromFile("assets/sprites/Abandoned_tent.png");
    // Nếu ảnh Start chưa có, dùng tạm Battle hoặc để trống
    nodeTextures[NodeType::Empty].loadFromFile("assets/sprites/Battle_cleared.png"); 
}

void Map::generateDefaultLayout() {
    nodes.clear();
    int id = 0;
    // Tạo lưới node 3 hàng x 5 cột
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 5; ++c) {
            Node n;
            n.id = id++;
            n.pos = { 150.f + c * 180.f, 150.f + r * 150.f }; // Tăng khoảng cách ra chút cho đẹp
            
            // Random loại node để map đa dạng hơn
            int randVal = Utils::randomInt(0, 100);
            if (randVal < 60) n.type = NodeType::Battle;       // 60% gặp quái
            else if (randVal < 80) n.type = NodeType::Bonfire; // 20% lửa trại
            else n.type = NodeType::Berry;                     // 20% lượm quả
            
            nodes.push_back(n);
        }
    }
    
    // Set cứng điểm đầu và cuối
    nodes[0].type = NodeType::Empty;  // Điểm xuất phát
    nodes.back().type = NodeType::Boss; // Điểm cuối là Boss
    
    currentNodeId = 0;
    nodes[0].visited = true;

    // Logic nối neighbors (giữ nguyên)
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 5; ++c) {
            int idx = r * 5 + c;
            if (c + 1 < 5) nodes[idx].neighbors.push_back(idx + 1);
            if (r + 1 < 3) nodes[idx].neighbors.push_back(idx + 5);
        }
    }
    
    // Gán stage ID cho Battle
    for (auto& n : nodes) {
        if (n.type == NodeType::Battle) n.stageID = Utils::randomInt(0, 5);
    }
}

void Map::render(sf::RenderWindow& win, sf::Font& font) {
    // 1. Vẽ đường nối trước
    sf::Vertex line[2];
    line[0].color = sf::Color(100, 100, 100);
    line[1].color = sf::Color(100, 100, 100);
    
    for (auto &n : nodes) {
        for (auto &nid : n.neighbors) {
            Node* neighbor = getNodeById(nid);
            if (neighbor) {
                line[0].position = n.pos; 
                line[1].position = neighbor->pos;
                win.draw(line, 2, sf::Lines);
            }
        }
    }
    
    // 2. Vẽ Node bằng SPRITE (Ảnh)
    for (auto &n : nodes) {
        sf::Sprite spr;
        
        // Kiểm tra nếu có texture cho loại node này thì dùng
        if (nodeTextures.count(n.type)) {
            spr.setTexture(nodeTextures[n.type]);
            
            // Scale ảnh nhỏ lại nếu ảnh gốc quá to (ví dụ ảnh 500px thì scale 0.15)
            spr.setScale(0.5f, 0.5f); 
            
            // Căn giữa ảnh vào vị trí node
            sf::FloatRect bounds = spr.getLocalBounds();
            spr.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
            spr.setPosition(n.pos);
            
            // Hiệu ứng màu: Vàng (đang đứng), Tối (chưa đến), Sáng (đã qua)
            if (n.id == currentNodeId) spr.setColor(sf::Color(255, 255, 150)); // Vàng sáng
            else if (!n.visited) spr.setColor(sf::Color(200, 200, 200));       // Hơi tối
            else spr.setColor(sf::Color::White);                               // Bình thường
            
            win.draw(spr);
        } else {
            // Fallback: Vẽ hình tròn nếu không tìm thấy ảnh
            sf::CircleShape c(20.f);
            c.setOrigin(20.f, 20.f);
            c.setPosition(n.pos);
            c.setFillColor(sf::Color::Red);
            win.draw(c);
        }
    }
    
    // Vẽ viền chọn xung quanh node hiện tại
    Node* cur = getNodeById(currentNodeId);
    if (cur) {
        sf::CircleShape ring(40.f); // Vòng tròn bao quanh
        ring.setOrigin(40.f, 40.f);
        ring.setPosition(cur->pos);
        ring.setFillColor(sf::Color::Transparent);
        ring.setOutlineColor(sf::Color::Yellow);
        ring.setOutlineThickness(3.f);
        win.draw(ring);
    }
}

// ... (Giữ nguyên các hàm clickNode, movePlayerToNode...)
// Nhớ giữ nguyên phần logic cũ ở dưới
bool Map::clickNode(const sf::Vector2f& mPos, int &outId) {
    for (auto &n : nodes) {
        // Tăng vùng click lên 40px vì ảnh icon to hơn chấm tròn
        if (Utils::pointInCircle(n.pos, 40.f, mPos)) { 
            Node* cur = getNodeById(currentNodeId);
            if (!cur || n.id == currentNodeId) return false;
            for (int nid : cur->neighbors) {
                if (n.id == nid) { outId = n.id; return true; }
            }
        }
    }
    return false;
}
// ... copy nốt các hàm getter ở cuối file cũ vào đây
void Map::movePlayerToNode(int id) { 
    currentNodeId = id; 
    Node* n = getNodeById(id);
    if(n) n->visited = true;
}
Node* Map::getNodeById(int id) {
    for (auto &n : nodes) if (n.id == id) return &n;
    return nullptr;
}
int Map::getCurrentNodeId() const { return currentNodeId; }