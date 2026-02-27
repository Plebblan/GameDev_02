#ifndef ARENA_H
#define ARENA_H

#include "game.h"
#include <cmath>

class Arena
{
public:
    Arena(int width = 1280, int height = 720, int wallThickness = 10);

    void Render(SDL_Renderer* renderer) const;

    // Checks collision and modifies velocity for ricochet
    void CheckCollision(SDL_FRect& ballRect, Vector2 &vel) const;

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

private:
    int m_width;
    int m_height;
    int m_wallThickness;

    SDL_FRect m_topWall;
    SDL_FRect m_bottomWall;
    SDL_FRect m_leftWall;
    SDL_FRect m_rightWall;
};

#endif