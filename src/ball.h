#ifndef BALL_H
#define BALL_H

#include "game.h"
#include "arena.h"

class Player;

enum class BallState
{
    Normal,
    Bunted,
    Caught
};

class Ball
{
public:
    Ball(Vector2 pos, float size = 50.0f);

    void Update(float deltaTime, Arena arena);
    void Render(SDL_Renderer* renderer) const;

    SDL_FRect& GetRect();

    Vector2& getVelocity();
    void setVelocity(Vector2 vel);

    void SetOwner(Player* player);
    Player* GetOwner() const;

    bool IsNeutral() const;

    void StartBunt(Player* bunter, AttackDirection dir);
    void UnBunt();

    void Catch(Player* player);
    void Throw(AttackDirection dir);
    bool IsCaught() const;
    Player* GetHolder();

private:
    SDL_FRect m_rect;
    Vector2 m_vel;
    float m_speed;
    Player* m_owner;
    float   m_neutralTimer;
    float   m_neutralDuration;

    BallState m_state;
    Player* m_holder;

    float m_buntTimer;
    float m_buntDuration;

    float m_buntHeight;     
    float m_buntDistance;   
    float m_preBuntSpeed;
    Player* m_preBuntOwner;
    };

#endif