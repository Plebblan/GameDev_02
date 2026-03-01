#ifndef ARENA_H
#define ARENA_H

#include "game.h"
#include <vector>
#include <cmath>
#include <algorithm>

class Arena
{
public:
    Arena(int ver = 0, int width = 1280, int height = 720, int wallThickness = 30);

    void Render(SDL_Renderer* renderer) const;

    // Checks collision and modifies velocity for ricochet
    void CheckCollision(SDL_FRect& ballRect, Vector2 &vel) const;

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    Vector2 getBallStart();

private:
    int m_width;
    int m_height;
    int m_wallThickness;
    Vector2 m_ballStartPos = Vector2(640.0f - 10.0f, 200.0f);

    std::vector<SDL_FRect> m_walls;
};



#endif