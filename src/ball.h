#ifndef BALL_H
#define BALL_H

#include <SDL3/SDL.h>

class Ball
{
public:
    Ball(float startX, float startY, float size = 20.0f);

    void Update(float deltaTime);
    void Render(SDL_Renderer* renderer) const;

    SDL_FRect& GetRect();
    float& GetVelX();
    float& GetVelY();

    void SetVelocity(float vx, float vy);

private:
    SDL_FRect m_rect;

    float m_velX;
    float m_velY;

    float m_speed;
};

#endif