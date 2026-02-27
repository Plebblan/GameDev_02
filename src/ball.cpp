#include "ball.h"
#include <cmath>

static constexpr float GRAVITY = 10.0f;        // Very weak gravity
static constexpr float BOUNCE_DAMPING = 0.995f; // Very small energy loss

Ball::Ball(float startX, float startY, float size)
    : m_velX(250.0f), m_velY(200.0f), m_speed(300.0f)
{
    m_rect = {
        startX,
        startY,
        size,
        size
    };
}

void Ball::Update(float deltaTime)
{
    // Apply weak gravity
    m_velY += GRAVITY * deltaTime;

    // Move
    m_rect.x += m_velX * deltaTime;
    m_rect.y += m_velY * deltaTime;
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

float& Ball::GetVelX()
{
    return m_velX;
}

float& Ball::GetVelY()
{
    return m_velY;
}

void Ball::SetVelocity(float vx, float vy)
{
    m_velX = vx;
    m_velY = vy;
}