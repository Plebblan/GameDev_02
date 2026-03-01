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
    m_font = TTF_OpenFont("/System/Library/Fonts/Supplemental/Arial.ttf", 20);
}

void Menu::InitializeMaps() {
    m_maps.clear();
    
    // Map 1: Classic Arena (without platforms)
    m_maps.push_back({
        0,  // ver
        "Classic Arena",
        1280,
        720,
        10,
        Vector2(640.0f - 20.0f, 200.0f),
        Vector2(600.0f, 500.0f),
        Vector2(400.0f, 500.0f),
        "Standard arena without platforms"
    });
    
    // Map 2: Arena with Platforms
    m_maps.push_back({
        1,  // ver
        "Platform Arena",
        1280,
        720,
        10,
        Vector2(640.0f - 20.0f, 200.0f),
        Vector2(600.0f, 500.0f),
        Vector2(400.0f, 500.0f),
        "Arena with middle platforms"
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
        m_selectionTimer = 1.0f; // Delay 300ms để không repeat quá nhanh
    }
    
    // Chuyển xuống (S hoặc Down Arrow)
    if ((keyboardState[SDL_SCANCODE_S] || keyboardState[SDL_SCANCODE_DOWN]) && m_selectionTimer <= 0.0f) {
        m_selectedIndex++;
        if (m_selectedIndex >= (int)m_maps.size())
            m_selectedIndex = 0;
        m_selectionTimer = 1.0f;
    }
    
    // Chọn (SPACE hoặc ENTER)
    if ((keyboardState[SDL_SCANCODE_SPACE] || keyboardState[SDL_SCANCODE_RETURN]) && m_selectionTimer <= 0.0f) {
        m_mapSelected = true;
        m_selectionTimer = 0.3f; // ngăn double-tap nhầm
    }
}

void Menu::DrawTitle(SDL_Renderer* renderer, int windowWidth, int y) const {
    SDL_SetRenderDrawColor(renderer, 255, 200, 0, 255);
    SDL_FRect titleBox = {
        static_cast<float>(windowWidth / 2 - 150),
        static_cast<float>(y - 20),
        300.0f,
        60.0f
    };
    SDL_RenderRect(renderer, &titleBox);
    
    // 2. Vẽ Chữ thật sự bằng SDL_ttf
    if (m_font) {
        SDL_Color white = { 255, 255, 255, 255 };
        std::string titleText = "MAP SELECTION";
        
        // Tạo Surface từ chữ
        SDL_Surface* surface = TTF_RenderText_Blended(m_font, titleText.c_str(), 0, white);
        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            if (texture) {
                float textW = static_cast<float>(surface->w);
                float textH = static_cast<float>(surface->h);
                
                // Căn giữa chữ vào trong titleBox
                SDL_FRect destRect = {
                    static_cast<float>(windowWidth / 2 - textW / 2),
                    static_cast<float>(y - textH / 2 + 10), // Điều chỉnh y cho khớp box
                    textW,
                    textH
                };
                
                SDL_RenderTexture(renderer, texture, NULL, &destRect);
                
                // Dọn dẹp
                SDL_DestroyTexture(texture);
            }
            SDL_DestroySurface(surface);
        }
    }
}

void Menu::DrawMapList(SDL_Renderer* renderer, int windowWidth, int windowHeight) const {
    if (m_maps.empty()) return;

    // Tính toán vị trí bắt đầu để căn giữa danh sách theo chiều dọc
    int itemHeight = 50;
    int spacing = 10;
    int totalHeight = m_maps.size() * (itemHeight + spacing);
    int startY = (windowHeight - totalHeight) / 2;
    
    for (size_t i = 0; i < m_maps.size(); ++i) {
        int y = startY + i * (itemHeight + spacing);
        
        SDL_FRect mapBox = {
            static_cast<float>(windowWidth / 2 - 200),
            static_cast<float>(y),
            400.0f,
            static_cast<float>(itemHeight)
        };
        
        if ((int)i == m_selectedIndex) {
            // Màu xanh cho map đang chọn
            SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255); 
            SDL_RenderFillRect(renderer, &mapBox);
            // Màu chữ đen trên nền xanh
            DrawString(renderer, m_maps[i].name, mapBox.x + 20, mapBox.y + 15, 1.0f, {0, 0, 0, 255});
        } else {
            // Viền xám cho các map còn lại
            SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
            SDL_RenderRect(renderer, &mapBox);
            // Màu chữ trắng/xám
            DrawString(renderer, m_maps[i].name, mapBox.x + 20, mapBox.y + 15, 1.0f, {200, 200, 200, 255});
        }
    }
    
    // --- Phần hiển thị thông tin chi tiết phía dưới ---
    int infoY = windowHeight - 120;
    SDL_FRect infoBox = { (float)(windowWidth/2 - 300), (float)infoY, 600.0f, 100.0f };
    
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderRect(renderer, &infoBox);
    
    // Vẽ mô tả của map đang được chọn
    const auto& selectedMap = m_maps[m_selectedIndex];
    DrawString(renderer, "Description: " + selectedMap.description, infoBox.x + 20, infoBox.y + 35, 0.8f, {255, 255, 255, 255});
}

void Menu::DrawString(SDL_Renderer* renderer, std::string text, float x, float y, float scale, SDL_Color color) const {
    // 1. Tạo Surface từ font (m_font là TTF_Font*)
    SDL_Surface* surface = TTF_RenderText_Blended(m_font, text.c_str(), 0, color);
    if (!surface) return;

    // 2. Chuyển sang Texture
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    
    // 3. Xác định kích thước chữ
    SDL_FRect dstRect = { x, y, (float)surface->w * scale, (float)surface->h * scale };
    
    // 4. Vẽ
    SDL_RenderTexture(renderer, texture, nullptr, &dstRect);
    
    // 5. Dọn dẹp bộ nhớ (Rất quan trọng để tránh leak RAM)
    SDL_DestroyTexture(texture);
    SDL_DestroySurface(surface);
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
