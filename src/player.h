#ifndef PLAYER_H
#define PLAYER_H

#include "game.h"
#include "ball.h"
#include <cmath>

class Ball;

class Player
{
public:
    Player(Vector2 pos, int No = 1);
    ~Player();
    void HandleInput(const bool* keyboardState);
    void Update(float deltaTime, int arenaWidth, int arenaHeight, int wallThickness);
    void Render(SDL_Renderer* renderer) const;
    void PerformAttack(Ball& ball);
    void Bunt(Ball& ball);
    bool Check_collision(Ball& ball);
    int GetPlayerNumber() const { return m_Noplayer; }
    bool IsDead();
    void Reset(Vector2 vec);
private:
    SDL_FRect m_rect;

    Vector2 m_vel;
    int m_Noplayer;
    int m_hp;
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
    float m_nameW = 0, m_nameH = 0;
    int m_maxHp = 200; // Để tính tỷ lệ % thanh máu
    char pNum[3];
    TTF_Font* font;
};

#endif