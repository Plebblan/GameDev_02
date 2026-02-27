#ifndef PLAYER_H
#define PLAYER_H

#include <SDL3/SDL.h>

class Ball;

enum class AttackDirection
{
    None,
    Left,
    Right,
    Up,
    Down
};

class Player
{
public:
    Player(float startX, float startY);

    void HandleInput(const bool* keyboardState);
    void Update(float deltaTime, int arenaWidth, int arenaHeight, int wallThickness);
    void Render(SDL_Renderer* renderer) const;
    void PerformAttack(Ball& ball);

private:
    SDL_FRect m_rect;

    float m_velX;
    float m_velY;

    float m_moveSpeed;
    float m_jumpStrength;
    float m_doubleJumpStrength;

    bool m_isGrounded;

    bool m_jumpHeld;
    float m_jumpHoldTime;

    AttackDirection m_facing;
    float m_attackCooldown;

    bool  m_isAttacking;
    float m_attackTimer;
    float m_attackDuration;

    int  m_jumpCount;
    int  m_maxJumps;
    bool m_jumpPressedLastFrame;
};

#endif