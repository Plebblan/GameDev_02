#include "arena.h"

static constexpr float BOUNCE_DAMPING = 0.995f;

Arena::Arena(int width, int height, int wallThickness)
    : m_width(width), m_height(height), m_wallThickness(wallThickness)
{
    // Top wall
    m_topWall = {
        0.0f,
        0.0f,
        static_cast<float>(m_width),
        static_cast<float>(m_wallThickness)
    };

    // Bottom wall
    m_bottomWall = {
        0.0f,
        static_cast<float>(m_height - m_wallThickness),
        static_cast<float>(m_width),
        static_cast<float>(m_wallThickness)
    };

    // Left wall
    m_leftWall = {
        0.0f,
        0.0f,
        static_cast<float>(m_wallThickness),
        static_cast<float>(m_height)
    };

    // Right wall
    m_rightWall = {
        static_cast<float>(m_width - m_wallThickness),
        0.0f,
        static_cast<float>(m_wallThickness),
        static_cast<float>(m_height)
    };
}

void Arena::Render(SDL_Renderer* renderer) const
{
    // Debug draw walls (red)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    SDL_RenderFillRect(renderer, &m_topWall);
    SDL_RenderFillRect(renderer, &m_bottomWall);
    SDL_RenderFillRect(renderer, &m_leftWall);
    SDL_RenderFillRect(renderer, &m_rightWall);
}

void Arena::CheckCollision(SDL_FRect& ballRect, float& velX, float& velY) const
{
    // Left wall
    if (ballRect.x <= m_wallThickness)
    {
        ballRect.x = static_cast<float>(m_wallThickness);
        velX = -velX * BOUNCE_DAMPING;
    }

    // Right wall
    if (ballRect.x + ballRect.w >= m_width - m_wallThickness)
    {
        ballRect.x = static_cast<float>(m_width - m_wallThickness - ballRect.w);
        velX = -velX * BOUNCE_DAMPING;
    }

    // Top wall
    if (ballRect.y <= m_wallThickness)
    {
        ballRect.y = static_cast<float>(m_wallThickness);
        velY = -velY * BOUNCE_DAMPING;
    }

    // Bottom wall
    if (ballRect.y + ballRect.h >= m_height - m_wallThickness)
    {
        ballRect.y = static_cast<float>(m_height - m_wallThickness - ballRect.h);
        velY = -velY * BOUNCE_DAMPING;
    }
}