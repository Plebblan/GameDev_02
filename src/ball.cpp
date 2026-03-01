#include "ball.h"
#include "player.h"
#include <cmath>

static constexpr float GRAVITY = 10.0f;        // Very weak gravity
static constexpr float BOUNCE_LOSS= 3.0f;     // Very small energy loss

Ball::Ball(Vector2 pos, float size)
    : m_vel(Vector2()),
      m_speed(0.0f),
      m_owner(nullptr),
      m_previousOwner(nullptr),
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

void Ball::Update(float deltaTime)
{
    // =========================
    // BUNT ARC TAKES CONTROL
    // =========================
    if (m_isBunted)
    {
        m_buntTimer += deltaTime;

        float t = m_buntTimer / m_buntDuration;

        if (t >= 1.0f)
        {
            // ---- Finish Bunt ----
            m_isBunted = false;

            m_owner = m_preBuntOwner;

            float burst = 500.0f;

            m_vel.x = 0.0f;
            m_vel.y = burst;   // straight downward spike

            m_preBuntOwner = nullptr;
            return;
        }

        // Horizontal interpolation
        m_rect.x = m_buntStartX + m_buntDistance * t;

        // Parabolic arc
        float arc = 4.0f * m_buntHeight * t * (1.0f - t);
        m_rect.y = m_buntStartY - arc;

        return;  // IMPORTANT: stop normal physics while bunted
    }

    // =========================
    // NORMAL NEUTRAL TIMER
    // =========================
    if (m_owner == nullptr && m_neutralTimer > 0.0f)
    {
        m_neutralTimer -= deltaTime;

        if (m_neutralTimer <= 0.0f)
        {
            m_owner = m_previousOwner;
            m_neutralTimer = 0.0f;
        }
    }

    // =========================
    // NORMAL PHYSICS
    // =========================
    m_vel.y += GRAVITY * deltaTime;

    m_rect.x += m_vel.x * deltaTime;
    m_rect.y += m_vel.y * deltaTime;
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
    m_isBunted = true;

    m_preBuntSpeed = std::sqrt(m_vel.x * m_vel.x + m_vel.y * m_vel.y);
    m_preBuntOwner = m_owner;

    m_owner = nullptr; // Neutral (purple)

    m_buntTimer = 0.0f;
    m_buntDuration = 1.0f;

    m_buntStartX = m_rect.x;
    m_buntStartY = m_rect.y;

    // Default arc sizes
    m_buntHeight = 45.0f;
    m_buntDistance = 80.0f;

    switch (dir)
    {
        case AttackDirection::Left:
            m_buntDistance = -80.0f;
            break;

        case AttackDirection::Right:
            m_buntDistance = 80.0f;
            break;

        case AttackDirection::Up:
            m_buntHeight = 80.0f;  // higher vertical arc
            m_buntDistance = 0.0f;  // no horizontal drift
            break;

        case AttackDirection::Down:
            // optional: short downward pop
            m_buntHeight = -30.0f;
            m_buntDistance = 0.0f;
            break;

        default:
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
    m_previousOwner = player;
    m_neutralTimer = 0.0f;
}

void Ball::EnterNeutral(float duration)
{
    if (m_owner != nullptr)
        m_previousOwner = m_owner;

    m_owner = nullptr;
    m_neutralTimer = duration;
}

bool Ball::IsNeutral() const
{
    return m_owner == nullptr;
}

Player* Ball::GetOwner() const
{
    return m_owner;
}