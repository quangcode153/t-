#include "Map.h"
#include "Utils.h"
#include <iostream>

using namespace std;

Map::Map() : currentNodeId(0), maxUnlockedNodeId(0), selectedNodeId(0) {
    loadTextures();
}

void Map::loadTextures() {
    // 1. Load Background Map MỚI
    if (!mapBgTex.loadFromFile("assets/sprites/backgroundman.png")) {
        cerr << "Map Error: Failed to load backgroundman.png\n";
    } else {
        mapBgSprite.setTexture(mapBgTex);
        // Scale ảnh cho vừa màn hình 1280x720 (nếu ảnh gốc khác size)
        // Giả sử game chạy 1280x720
        mapBgSprite.setScale(
            1280.f / mapBgTex.getSize().x, 
            720.f / mapBgTex.getSize().y
        );
    }

    // 2. Load Icon Node (Giữ nguyên)
    if (!nodeTextures[NodeType::Battle].loadFromFile("assets/sprites/Battle.png")) {}
    nodeTextures[NodeType::Boss].loadFromFile("assets/sprites/Boss.png");
    nodeTextures[NodeType::Bonfire].loadFromFile("assets/sprites/Bonfire.png");
    nodeTextures[NodeType::Berry].loadFromFile("assets/sprites/Broodberries.png");
    nodeTextures[NodeType::Shop].loadFromFile("assets/sprites/Abandoned_tent.png");
    nodeTextures[NodeType::Empty].loadFromFile("assets/sprites/Battle_cleared.png"); 
}

void Map::generateDefaultLayout() {
    nodes.clear();
    int id = 0;
    
    // --- CĂN CHỈNH VỊ TRÍ NODE CHO KHỚP VỚI CUỘN GIẤY ---
    // Phần giấy cuộn thường nằm ở giữa theo chiều dọc -> Y khoảng 360-380
    // Chiều ngang bắt đầu từ khoảng 150 đến 1130 để tránh mép cuộn giấy
    
    float startX = 240.f; // Dịch vào trong một chút để tránh mép trái cuộn giấy
    float startY = 360.f; // Giữa màn hình (trục dọc của cuộn giấy)
    float gapX = 170.f;   // Khoảng cách giữa các node

    for (int i = 0; i < 6; ++i) {
        Node n;
        n.id = id++;
        
        // Đặt vị trí
        n.pos = { startX + i * gapX, startY }; 
        
        // Logic loại node (giữ nguyên)
        if (i == 5) n.type = NodeType::Boss;
        else n.type = NodeType::Battle;

        n.stageID = i; 
        nodes.push_back(n);
    }
    
    currentNodeId = 0;
    maxUnlockedNodeId = 0; 
    selectedNodeId = 0;    
}

void Map::unlockNextNode() {
    if (maxUnlockedNodeId < (int)nodes.size() - 1) {
        maxUnlockedNodeId++;
    }
    selectedNodeId = maxUnlockedNodeId;
    currentNodeId = maxUnlockedNodeId;
}

bool Map::canEnterSelectedNode() const {
    return selectedNodeId <= maxUnlockedNodeId;
}

bool Map::clickNode(const sf::Vector2f& mPos) {
    for (auto &n : nodes) {
        if (Utils::pointInCircle(n.pos, 45.f, mPos)) {
            if (n.id <= maxUnlockedNodeId) {
                selectedNodeId = n.id;
                return true;
            }
        }
    }
    return false;
}

void Map::render(sf::RenderWindow& win, sf::Font& font) {
    // 1. VẼ BACKGROUND TRƯỚC (MỚI)
    win.draw(mapBgSprite);

    // 2. VẼ ĐƯỜNG NỐI (Giờ chuyển màu đường nối sang màu Đen/Nâu cho hợp style giấy)
    sf::Vertex line[2];
    for (size_t i = 0; i < nodes.size() - 1; ++i) {
        line[0].position = nodes[i].pos;
        line[1].position = nodes[i+1].pos;
        
        if (i < (size_t)maxUnlockedNodeId) {
            // Đã mở: Màu Đen đậm (như mực vẽ trên giấy)
            line[0].color = sf::Color(50, 30, 10); 
            line[1].color = sf::Color(50, 30, 10);
        } else {
            // Chưa mở: Màu Nâu nhạt mờ
            line[0].color = sf::Color(180, 160, 140, 100);
            line[1].color = sf::Color(180, 160, 140, 100);
        }
        win.draw(line, 2, sf::Lines);
    }

    // 3. VẼ CÁC NODE
    for (auto &n : nodes) {
        sf::Sprite spr;
        
        if (nodeTextures.count(n.type)) {
            spr.setTexture(nodeTextures[n.type]);
            spr.setOrigin(spr.getLocalBounds().width / 2.f, spr.getLocalBounds().height / 2.f);
            spr.setPosition(n.pos);
            spr.setScale(0.5f, 0.5f);

            if (n.id > maxUnlockedNodeId) {
                // Chưa mở khóa: Màu xám đen (trông như chưa tô màu)
                spr.setColor(sf::Color(80, 80, 80, 180)); 
            } else {
                if (n.id == selectedNodeId) {
                    spr.setColor(sf::Color::White);
                    spr.setScale(0.6f, 0.6f);
                    
                    // Highlight: Dùng màu Vàng cam cho hợp tông giấy
                    sf::CircleShape ring(50.f);
                    ring.setOrigin(50.f, 50.f);
                    ring.setPosition(n.pos);
                    ring.setFillColor(sf::Color::Transparent);
                    ring.setOutlineColor(sf::Color(255, 200, 0)); // Vàng cam
                    ring.setOutlineThickness(4.f);
                    win.draw(ring);
                } else {
                    // Đã mở nhưng không chọn: Hơi tối chút
                    spr.setColor(sf::Color(200, 200, 200));
                }
            }
            win.draw(spr);
        } 
        else {
            sf::CircleShape c(20.f);
            c.setOrigin(20.f, 20.f);
            c.setPosition(n.pos);
            c.setFillColor(n.id <= maxUnlockedNodeId ? sf::Color::Red : sf::Color(100, 100, 100));
            win.draw(c);
        }
        
        // Vẽ số màn chơi
        if (n.id <= maxUnlockedNodeId) {
            sf::Text t(Utils::toString(n.id + 1), font, 20);
            t.setOrigin(t.getLocalBounds().width/2, 0);
            t.setPosition(n.pos.x, n.pos.y + 35);
            t.setFillColor(sf::Color::Black); // Chữ đen trên nền giấy
            // t.setOutlineColor(sf::Color::White); // Có thể thêm viền trắng nếu cần
            // t.setOutlineThickness(1.f);
            win.draw(t);
        }
    }
}

// ... (Giữ nguyên các hàm getter cuối file) ...
void Map::movePlayerToNode(int id) { currentNodeId = id; }
Node* Map::getNodeById(int id) { for(auto& n : nodes) if(n.id == id) return &n; return nullptr; }
int Map::getCurrentNodeId() const { return currentNodeId; }