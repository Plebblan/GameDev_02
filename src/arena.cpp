#include "arena.h"

static constexpr float BOUNCE_LOSS = 5.0f;

Arena::Arena(int ver, int width, int height, int wallThickness)
    : m_width(width), m_height(height), m_wallThickness(wallThickness)
{
    // Top wall
    m_walls.push_back({
        0.0f,
        0.0f,
        static_cast<float>(m_width),
        static_cast<float>(m_wallThickness)
    });

    // Bottom wall
    m_walls.push_back({
        0.0f,
        static_cast<float>(m_height - m_wallThickness),
        static_cast<float>(m_width),
        static_cast<float>(m_wallThickness)
    });

    // Left wall
    m_walls.push_back({
        0.0f,
        0.0f,
        static_cast<float>(m_wallThickness),
        static_cast<float>(m_height)
    });

    // Right wall
    m_walls.push_back({
        static_cast<float>(m_width - m_wallThickness),
        0.0f,
        static_cast<float>(m_wallThickness),
        static_cast<float>(m_height)
    });
    if (ver == 1){
        m_walls.push_back({
            static_cast<float>(180),
            static_cast<float>(480),
            static_cast<float>(345),
            static_cast<float>(m_wallThickness)
        });
        m_walls.push_back({
            static_cast<float>(825),
            static_cast<float>(480),
            static_cast<float>(345),
            static_cast<float>(m_wallThickness)
        });
    }
}

void Arena::Render(SDL_Renderer* renderer) const
{
    // Debug draw walls (red)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (auto it : m_walls){
        SDL_RenderFillRect(renderer, &it);
    }
}

Vector2 Arena::getBallStart(){
    return m_ballStartPos;
}

void Arena::CheckCollision(SDL_FRect& ballRect, Vector2 &vel) const
{
    float radius = ballRect.h / 2.0f;
    Vector2 ballPos = Vector2(ballRect.x + radius, ballRect.y + radius);
    for (auto it : m_walls){
        //First check for collision
        if (
            (ballPos.x - radius <= it.x + it.w && ballPos.x + radius >= it.x)
            && (ballPos.y - radius <= it.y + it.h && ballPos.y + radius >= it.y)
        ){
            Vector2 normal;
            float leftDist   = std::abs(ballPos.x - it.x);
            float rightDist  = std::abs(ballPos.x - (it.x + it.w));
            float topDist    = std::abs(ballPos.y - it.y);
            float bottomDist = std::abs(ballPos.y - (it.y + it.h));
            
            float minDist = std::min({leftDist, rightDist, topDist, bottomDist});
            
            if (minDist == leftDist) {
                normal = Vector2(-1.0f, 0.0f);
                ballRect.x = it.x - ballRect.h;
            }
            else if (minDist == rightDist) {
                normal = Vector2(1.0f, 0.0f); 
                ballRect.x = it.x + it.w;
            }
            else if (minDist == topDist) {
                normal = Vector2(0.0f, -1.0f);
                ballRect.y = it.y - ballRect.h;
            }
            else {
                normal = Vector2(0.0f, 1.0f);
                ballRect.y = it.y + it.h;
            }
            double angle = vel.angle(normal);
            double d_angle = 180.0 -  (180.0 - angle) * 2;
            vel.rotate(d_angle);
            float speed = std::max(vel.length() - BOUNCE_LOSS, 0.0f);
            vel.normalize();
            vel.mul(speed);
        }
    }
}