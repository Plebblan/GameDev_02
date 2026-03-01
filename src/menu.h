#pragma once

#ifndef MENU_H
#define MENU_H

#include "game.h"
#include <vector>
#include <string>

// Cấu hình cho từng Map
struct MapConfig {
    int ver;                    // Phiên bản Arena (0 = no platforms, 1 = with platforms)
    std::string name;           // Tên Map
    int width;                  // Chiều rộng
    int height;                 // Chiều cao
    int wallThickness;          // Độ dày tường
    Vector2 ballSpawnPos;       // Vị trí bắt đầu bóng
    Vector2 player1SpawnPos;    // Vị trí bắt đầu Player 1
    Vector2 player2SpawnPos;    // Vị trí bắt đầu Player 2
    std::string description;    // Mô tả Map
};

class Menu {
public:
    Menu();
    
    // Khởi tạo các Map có sẵn
    void InitializeMaps();
    
    // Xử lý input từ bàn phím
    void HandleInput(const bool* keyboardState);
    
    // Vẽ Menu
    void Render(SDL_Renderer* renderer, int windowWidth, int windowHeight) const;
    
    // Kiểm tra xem user đã chọn Map chưa
    bool IsMapSelected() const { return m_mapSelected; }
    
    // Lấy Map được chọn
    const MapConfig& GetSelectedMap() const { return m_maps[m_selectedIndex]; }
    
    // Reset Menu
    void Reset() { m_mapSelected = false; m_selectedIndex = 0; }
    
private:
    std::vector<MapConfig> m_maps;
    int m_selectedIndex;        // Index của Map hiện tại đang được highlight
    bool m_mapSelected;         // Có chọn Map hay không
    float m_selectionTimer;     // Timer để tránh input lặp lại quá nhanh
    TTF_Font * m_font;
    void DrawString(SDL_Renderer* renderer, std::string text, float x, float y, float scale, SDL_Color color) const;
    // Các hàm hỗ trợ vẽ
    void DrawTitle(SDL_Renderer* renderer, int windowWidth, int y) const;
    void DrawMapList(SDL_Renderer* renderer, int windowWidth, int windowHeight) const;
    void DrawInstructions(SDL_Renderer* renderer, int windowWidth, int windowHeight) const;
};

#endif
