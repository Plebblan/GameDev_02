#include "ball.h"
#include "player.h"
#include <cmath>

static constexpr float GRAVITY = 10.0f;        // Very weak gravity
static constexpr float BOUNCE_LOSS= 3.0f;     // Very small energy loss

Ball::Ball(Vector2 pos, float size)
    : m_vel(Vector2()),
      m_speed(0.0f),
      m_owner(nullptr),
      m_preBuntOwner(nullptr),
      m_neutralTimer(9999.0f),
      m_isBunted(false),
      m_buntTimer(0.0f),
      m_buntDuration(0.0f)
{
    m_rect = {
        pos.x,
        pos.y,
        size,
        size
    };
}

void Ball::Update(float dt)
{
    if (m_isBunted)
    {
        m_buntTimer += dt;

        if (m_buntTimer >= m_buntDuration)
        {
            m_isBunted = false;

            m_owner = m_preBuntOwner;

            float burst = 500.0f;

            m_vel.x = 0.0f;
            m_vel.y = burst; // spike down
            m_preBuntOwner = nullptr;
        }
        m_vel.y += 100.0f * dt;
    }
    else
        m_vel.y += GRAVITY * dt;

    m_rect.x += m_vel.x * dt;
    m_rect.y += m_vel.y * dt;
}

void Ball::Render(SDL_Renderer* renderer) const
{
    // ---- Determine color based on ownership ----
    if (IsNeutral())
    {
        SDL_SetRenderDrawColor(renderer, 180, 0, 255, 255); // Purple
    }
    else if (m_owner != nullptr)
    {
        if (m_owner->GetPlayerNumber() == 1)
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);   // Red
        else
            SDL_SetRenderDrawColor(renderer, 0, 120, 255, 255); // Blue
    }
    else
    {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Fallback white
    }

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

void Ball::StartBunt(Player* bunter, AttackDirection dir)
{
    if (!m_isBunted)
    {
        m_preBuntOwner = m_owner;
    }

    m_isBunted = true;
    m_owner = nullptr; // neutral (purple)

    m_buntTimer = 0.0f;
    m_buntDuration = 1.0f;

    float buntSpeed = 80.0f;
    float upwardForce = -50.0f; // upward in SDL is negative Y

    switch (dir)
    {
        case AttackDirection::Left:
            m_vel.x = -buntSpeed;
            m_vel.y = upwardForce;
            break;

        case AttackDirection::Right:
            m_vel.x = buntSpeed;
            m_vel.y = upwardForce;
            break;

        case AttackDirection::Up:
            m_vel.x = 0.0f;
            m_vel.y = upwardForce * 1.2f; // more vertical arc
            break;

        default:
            m_vel.x = 0.0f;
            m_vel.y = upwardForce;
            break;
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

void Ball::SetOwner(Player* player)
{
    m_owner = player;
    m_neutralTimer = 0.0f;
}

bool Ball::IsNeutral() const
{
    return m_owner == nullptr;
}

Player* Ball::GetOwner() const
{
    return m_owner;
}