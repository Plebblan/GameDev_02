#ifndef BALL_H
#define BALL_H

#include "game.h"

class Ball
{
public:
    Ball(Vector2 pos, float size = 20.0f);

    void Update(float deltaTime);
    void Render(SDL_Renderer* renderer) const;

    SDL_FRect& GetRect();
    Vector2& getVelocity();

    void setVelocity(Vector2 vel);

private:
    SDL_FRect m_rect;
    Vector2 m_vel;
    float m_speed;
};

#endif