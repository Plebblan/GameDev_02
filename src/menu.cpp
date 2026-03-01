#include "menu.h"
#include <cmath>
#include <iostream>

// Font helpers cho vẽ text đơn giản (dùng hình học)
// Hiện tại chỉ vẽ mỗi ký tự dưới dạng khung chữ nhật, đủ cho danh sách

void DrawChar(SDL_Renderer* renderer, char c, float x, float y, float scale)
{
    // ký tự c hiện chưa dùng, nhưng giữ tham số để dễ mở rộng font
    (void)c;
    SDL_FRect rect = { x, y, 8.0f * scale, 20.0f * scale };
    SDL_RenderRect(renderer, &rect);
}

void DrawString(SDL_Renderer* renderer, const std::string& str, float x, float y, float scale)
{
    float penX = x;
    for (char c : str) {
        if (c == ' ') {
            penX += 10.0f * scale;
            continue;
        }
        DrawChar(renderer, c, penX, y, scale);
        penX += 10.0f * scale;
    }
}

Menu::Menu()
    : m_selectedIndex(0),
      m_mapSelected(false),
      m_selectionTimer(0.0f)
{
    InitializeMaps();
}

void Menu::InitializeMaps() {
    m_maps.clear();
    
    // Map 1: Classic Arena
    m_maps.push_back({
        "Classic Arena",
        1280,
        720,
        10,
        Vector2(640.0f - 10.0f, 200.0f),
        Vector2(600.0f, 500.0f),
        Vector2(400.0f, 500.0f),
        "Standard 1280x720 arena"
    });
    
    // Map 2: Tall Arena
    m_maps.push_back({
        "Tall Arena",
        800,
        1000,
        10,
        Vector2(400.0f - 10.0f, 300.0f),
        Vector2(350.0f, 700.0f),
        Vector2(250.0f, 700.0f),
        "Tall and narrow 800x1000"
    });
    
    // Map 3: Wide Arena
    m_maps.push_back({
        "Wide Arena",
        1600,
        600,
        10,
        Vector2(800.0f - 10.0f, 150.0f),
        Vector2(750.0f, 450.0f),
        Vector2(550.0f, 450.0f),
        "Wide and short 1600x600"
    });
    
    // Map 4: Small Arena
    m_maps.push_back({
        "Small Arena",
        640,
        480,
        8,
        Vector2(320.0f - 5.0f, 100.0f),
        Vector2(300.0f, 350.0f),
        Vector2(200.0f, 350.0f),
        "Compact 640x480 arena"
    });
    
    // Map 5: Big Arena
    m_maps.push_back({
        "Big Arena",
        1920,
        1080,
        12,
        Vector2(960.0f - 10.0f, 300.0f),
        Vector2(900.0f, 800.0f),
        Vector2(700.0f, 800.0f),
        "Large 1920x1080 arena"
    });
}

void Menu::HandleInput(const bool* keyboardState) {
    // Giảm timer
    if (m_selectionTimer > 0.0f)
        m_selectionTimer -= 0.016f;
    
    // Chuyển lên (W hoặc Up Arrow)
    if ((keyboardState[SDL_SCANCODE_W] || keyboardState[SDL_SCANCODE_UP]) && m_selectionTimer <= 0.0f) {
        m_selectedIndex--;
        if (m_selectedIndex < 0)
            m_selectedIndex = m_maps.size() - 1;
        m_selectionTimer = 0.3f; // Delay 300ms để không repeat quá nhanh
    }
    
    // Chuyển xuống (S hoặc Down Arrow)
    if ((keyboardState[SDL_SCANCODE_S] || keyboardState[SDL_SCANCODE_DOWN]) && m_selectionTimer <= 0.0f) {
        m_selectedIndex++;
        if (m_selectedIndex >= (int)m_maps.size())
            m_selectedIndex = 0;
        m_selectionTimer = 0.3f;
    }
    
    // Chọn (SPACE hoặc ENTER)
    if ((keyboardState[SDL_SCANCODE_SPACE] || keyboardState[SDL_SCANCODE_RETURN]) && m_selectionTimer <= 0.0f) {
        m_mapSelected = true;
        m_selectionTimer = 0.3f; // ngăn double-tap nhầm
    }
}

void Menu::DrawTitle(SDL_Renderer* renderer, int windowWidth, int y) const {
    SDL_SetRenderDrawColor(renderer, 255, 200, 0, 255);
    
    // Vẽ một rectangle highlight cho title
    SDL_FRect titleBox = {
        static_cast<float>(windowWidth / 2 - 150),
        static_cast<float>(y - 20),
        300.0f,
        60.0f
    };
    SDL_RenderRect(renderer, &titleBox);
    
    // Vẽ text "MAP SELECTION" bằng cách vẽ các khối chữ đơn giản
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    std::string title = "MAP SELECTION";
    int titleLen = (int)title.size();
    int startX = windowWidth / 2 - (titleLen * 10) / 2;
    
    for (int i = 0; i < titleLen; ++i) {
        int charX = startX + i * 12;
        SDL_FRect charRect = { static_cast<float>(charX), static_cast<float>(y), 8.0f, 20.0f };
        SDL_RenderRect(renderer, &charRect);
    }
}

void Menu::DrawMapList(SDL_Renderer* renderer, int windowWidth, int windowHeight) const {
    int startY = windowHeight / 2 - (m_maps.size() * 50) / 2;
    
    for (size_t i = 0; i < m_maps.size(); ++i) {
        int y = startY + i * 60;
        
        SDL_FRect mapBox = {
            static_cast<float>(windowWidth / 2 - 200),
            static_cast<float>(y),
            400.0f,
            50.0f
        };
        
        if ((int)i == m_selectedIndex) {
            // Highlight map được chọn
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            SDL_RenderFillRect(renderer, &mapBox);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
            SDL_RenderRect(renderer, &mapBox);
            SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        }
        
        // Vẽ tên map
        const auto& map = m_maps[i];
        float text_x = windowWidth / 2 - 180;
        float text_y = y + 20;
        DrawString(renderer, map.name, text_x, text_y, 1.0f);
    }
    
    // Vẽ text thông tin phía dưới
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    const auto& selectedMap = m_maps[m_selectedIndex];
    
    int infoY = windowHeight - 120;
    SDL_FRect infoBox = {
        static_cast<float>(windowWidth / 2 - 300),
        static_cast<float>(infoY),
        600.0f,
        100.0f
    };
    SDL_RenderRect(renderer, &infoBox);
    
    // Vẽ mô tả map
    DrawString(renderer, selectedMap.description, windowWidth / 2 - 280, infoY + 40, 0.8f);
}

void Menu::DrawInstructions(SDL_Renderer* renderer, int windowWidth, int windowHeight) const {
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    
    int instructY = windowHeight - 50;
    
    // Vẽ các đường để chỉ dẫn
    SDL_RenderLine(renderer, windowWidth / 2 - 250, instructY, windowWidth / 2 + 250, instructY);
    
    // Vẽ các điểm để biểu thị text
    for (int i = 0; i < 20; ++i) {
        SDL_RenderPoint(renderer, windowWidth / 2 - 200 + i * 10, instructY + 15);
    }
}

void Menu::Render(SDL_Renderer* renderer, int windowWidth, int windowHeight) const {
    // Vẽ background
    SDL_SetRenderDrawColor(renderer, 30, 30, 40, 255);
    SDL_RenderClear(renderer);
    
    // Vẽ các thành phần Menu
    DrawTitle(renderer, windowWidth, 80);
    DrawMapList(renderer, windowWidth, windowHeight);
    DrawInstructions(renderer, windowWidth, windowHeight);
    
    // Vẽ border chính
    SDL_SetRenderDrawColor(renderer, 100, 100, 200, 255);
    SDL_FRect borderBox = { 50.0f, 50.0f, static_cast<float>(windowWidth - 100), static_cast<float>(windowHeight - 100) };
    SDL_RenderRect(renderer, &borderBox);
}
