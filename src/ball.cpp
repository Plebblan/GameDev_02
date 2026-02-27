#include "ball.h"
#include <cmath>

static constexpr float GRAVITY = 10.0f;        // Very weak gravity
static constexpr float BOUNCE_LOSS= 5.0f;     // Very small energy loss

Ball::Ball(Vector2 pos, float size)
    : m_vel(Vector2()), m_speed(0.0f)
{
    m_rect = {
        pos.x,
        pos.y,
        size,
        size
    };
}

void Ball::Update(float deltaTime)
{
    // Apply weak gravity
    m_vel.y += GRAVITY * deltaTime;

    // Move
    m_rect.x += m_vel.x * deltaTime;
    m_rect.y += m_vel.y * deltaTime;
}

void Ball::Render(SDL_Renderer* renderer) const
{
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

    float radius = m_rect.w / 2.0f;
    float centerX = m_rect.x + radius;
    float centerY = m_rect.y + radius;

    for (int w = 0; w < radius * 2; w++)
    {
        for (int h = 0; h < radius * 2; h++)
        {
            float dx = radius - w;
            float dy = radius - h;

            if ((dx * dx + dy * dy) <= (radius * radius))
            {
                SDL_RenderPoint(renderer,
                    static_cast<int>(centerX + dx),
                    static_cast<int>(centerY + dy));
            }
        }
    }
}

SDL_FRect& Ball::GetRect()
{
    return m_rect;
}

Vector2& Ball::getVelocity(){
    return m_vel;
}

void Ball::setVelocity(Vector2 vel)
{
    m_vel.set(vel);
}