#include "player.h"

static constexpr float GRAVITY = 900.0f;
static constexpr float HOLD_GRAVITY = 400.0f;   // Reduced gravity while holding jump
static constexpr float MAX_HOLD_TIME = 0.25f;   // Max time jump can be extended
static constexpr float JUMP_CUT_MULTIPLIER = 0.5f; // Cuts upward velocity if released early
static constexpr int BASE_HIT_DAMAGE = 50; 

// Helper function
static bool RectsIntersect(const SDL_FRect& a, const SDL_FRect& b)
{
    return (
        a.x < b.x + b.w &&
        a.x + a.w > b.x &&
        a.y < b.y + b.h &&
        a.y + a.h > b.y
    );
}

Player::Player(Vector2 pos, int No)
    : m_vel(Vector2()),
      m_moveSpeed(300.0f),
      m_jumpStrength(500.0f),
      m_doubleJumpStrength(350.0f),
      m_isGrounded(false),
      m_jumpHeld(false),
      m_jumpHoldTime(0.0f),
      m_facing(AttackDirection::Right),
      m_attackCooldown(0.0f),
      m_isAttacking(false),
      m_attackTimer(0.0f),
      m_attackDuration(0.15f),
      m_jumpCount(0),
      m_maxJumps(2),
      m_Noplayer(No),
      m_jumpPressedLastFrame(false)
{
    m_rect = { pos.x, pos.y, 20.0f, 60.0f };
    m_hp = 200;
}

void Player::HandleInput(const bool* keyboardState)
{   
    bool left, right, up, down, jump;
    if (m_Noplayer == 1){
        left  = keyboardState[SDL_SCANCODE_A];
        right = keyboardState[SDL_SCANCODE_D];
        up    = keyboardState[SDL_SCANCODE_W];
        down  = keyboardState[SDL_SCANCODE_S];
        jump  = keyboardState[SDL_SCANCODE_SPACE];
    }
    else {
        left  = keyboardState[SDL_SCANCODE_LEFT];   // Phím mũi tên trái
        right = keyboardState[SDL_SCANCODE_RIGHT];  // Phím mũi tên phải
        up    = keyboardState[SDL_SCANCODE_UP];     // Phím mũi tên lên
        down  = keyboardState[SDL_SCANCODE_DOWN];   // Phím mũi tên xuống
        jump  = keyboardState[SDL_SCANCODE_SLASH];  // Phím gạch chéo (/)
    }
     
    m_vel.x = 0.0f;

    // Movement + horizontal facing memory
    if (left)
    {
        m_vel.x = -m_moveSpeed;
        m_facing = AttackDirection::Left;
    }
    else if (right)
    {
        m_vel.x = m_moveSpeed;
        m_facing = AttackDirection::Right;
    }

    // Vertical overrides horizontal only while held
    if (up)
    {
        m_facing = AttackDirection::Up;
    }
    else if (down)
    {
        m_facing = AttackDirection::Down;
    }

    // ===== Jump logic =====
    if (jump && !m_jumpPressedLastFrame)
    {
        if (m_isGrounded || m_jumpCount < m_maxJumps)
        {
            if (m_jumpCount == 0)
            {
                // Ground jump
                m_vel.y = -m_jumpStrength;
            }
            else
            {
                // Air jump (weaker)
                m_vel.y = 0.0f;
                m_vel.y = -m_doubleJumpStrength;
            }
            m_isGrounded = false;
            m_jumpHeld = true;
            m_jumpHoldTime = 0.0f;
            m_jumpCount++;
        }
    }

    // Variable jump release
    if (!jump && m_jumpHeld && m_vel.y < 0.0f)
    {
        m_vel.y *= JUMP_CUT_MULTIPLIER;
        m_jumpHeld = false;
    }

    m_jumpPressedLastFrame = jump;
}

void Player::Update(float deltaTime, int arenaWidth, int arenaHeight, int wallThickness)
{
    float previousBottom = m_rect.y + m_rect.h;
    
    // Reduce attack cooldown
    if (m_attackCooldown > 0.0f)
        m_attackCooldown -= deltaTime;

    // Attack animation
    if (m_isAttacking)
    {
        m_attackTimer -= deltaTime;
        if (m_attackTimer <= 0.0f)
            m_isAttacking = false;
    }

    // Apply gravity
    if (m_jumpHeld && m_jumpHoldTime < MAX_HOLD_TIME)
    {
        m_vel.y += HOLD_GRAVITY * deltaTime;
        m_jumpHoldTime += deltaTime;
    }
    else
    {
        m_vel.y += GRAVITY * deltaTime;
    }

    // Move
    m_rect.x += m_vel.x * deltaTime;
    m_rect.y += m_vel.y * deltaTime;

    float floorY   = arenaHeight - wallThickness;
    float ceilingY = wallThickness;
    float leftWall = wallThickness;
    float rightWall = arenaWidth - wallThickness;

    bool wasGrounded = m_isGrounded;
    m_isGrounded = false;

    // ---------------- FLOOR ----------------
    float currentBottom = m_rect.y + m_rect.h;

    // Only land if falling and crossing the floor
    if (m_vel.y >= 0.0f &&
        previousBottom <= floorY &&
        currentBottom >= floorY)
    {
        m_rect.y = floorY - m_rect.h;
        m_vel.y = 0.0f;
        m_isGrounded = true;

        if (!wasGrounded)
        {
            m_jumpCount = 0;
            m_jumpHeld = false;
            m_jumpPressedLastFrame = false;
        }
    }

    // ---------------- CEILING ----------------
    if (m_rect.y <= ceilingY)
    {
        m_rect.y = ceilingY;

        if (m_vel.y < 0.0f)
            m_vel.y = 0.0f;
    }

    // ---------------- LEFT WALL ----------------
    if (m_rect.x <= leftWall)
    {
        m_rect.x = leftWall;
    }

    // ---------------- RIGHT WALL ----------------
    if (m_rect.x + m_rect.w >= rightWall)
    {
        m_rect.x = rightWall - m_rect.w;
    }
}

void Player::Render(SDL_Renderer* renderer) const
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    float centerX = m_rect.x + m_rect.w / 2.0f;

    SDL_FRect head = { centerX - 5.0f, m_rect.y, 10.0f, 10.0f };
    SDL_RenderFillRect(renderer, &head);

    SDL_RenderLine(renderer,
        centerX, m_rect.y + 10.0f,
        centerX, m_rect.y + 40.0f);

    SDL_RenderLine(renderer,
        centerX - 10.0f, m_rect.y + 20.0f,
        centerX + 10.0f, m_rect.y + 20.0f);

    SDL_RenderLine(renderer,
        centerX, m_rect.y + 40.0f,
        centerX - 10.0f, m_rect.y + 60.0f);

    SDL_RenderLine(renderer,
        centerX, m_rect.y + 40.0f,
        centerX + 10.0f, m_rect.y + 60.0f);

    if (m_isAttacking)
    {
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);

        const float radius = 100.0f;
        const int segments = 20;

        float centerX = m_rect.x + m_rect.w / 2.0f;
        float centerY = m_rect.y + m_rect.h / 2.0f;

        float startAngle = 0.0f;
        float endAngle = 0.0f;

        switch (m_facing)
        {
            case AttackDirection::Right:
                startAngle = -0.5f;
                endAngle   = 0.5f;
                break;
            case AttackDirection::Left:
                startAngle = 2.64f;   // approx pi - 0.5
                endAngle   = 3.64f;   // approx pi + 0.5
                break;
            case AttackDirection::Up:
                startAngle = -2.07f;  // approx -pi/2 - 0.5
                endAngle   = -1.07f;  // approx -pi/2 + 0.5
                break;
            case AttackDirection::Down:
                startAngle = 1.07f;   // approx pi/2 - 0.5
                endAngle   = 2.07f;   // approx pi/2 + 0.5
                break;
        }

        float step = (endAngle - startAngle) / segments;

        for (int i = 0; i < segments; ++i)
        {
            float a1 = startAngle + step * i;
            float a2 = startAngle + step * (i + 1);

            float x1 = centerX + cosf(a1) * radius;
            float y1 = centerY + sinf(a1) * radius;

            float x2 = centerX + cosf(a2) * radius;
            float y2 = centerY + sinf(a2) * radius;

            SDL_RenderLine(renderer, x1, y1, x2, y2);
        }
    }
}

void Player::PerformAttack(Ball& ball)
{
    if (m_attackCooldown > 0.0f)
        return;

    m_isAttacking = true;
    m_attackTimer = m_attackDuration;

    const float ATTACK_RADIUS = 100.0f;
    const float IMPULSE_STRENGTH = 400.0f;
    const float MAX_SPEED = 2000.0f;

    // Player center
    float px = m_rect.x + m_rect.w * 0.5f;
    float py = m_rect.y + m_rect.h * 0.5f;

    // Ball center
    SDL_FRect& ballRect = ball.GetRect();
    float bx = ballRect.x + ballRect.w * 0.5f;
    float by = ballRect.y + ballRect.h * 0.5f;

    float dx = bx - px;
    float dy = by - py;

    float distance = std::sqrt(dx * dx + dy * dy);

    if (distance > ATTACK_RADIUS || distance <= 0.01f)
    {
        m_attackCooldown = 0.3f;
                    return;
    }
    switch (m_facing)
        {
            case AttackDirection::Right:
                if ( float(dx) / distance < float(m_rect.w) / std::sqrt(m_rect.w*m_rect.w + m_rect.h*m_rect.w)){
                    m_attackCooldown = 0.3f;
                    return;
                }
                break;
            case AttackDirection::Left:
                if ( float(dx) / distance > float(m_rect.w) / std::sqrt(m_rect.w*m_rect.w + m_rect.h*m_rect.w)){
                    m_attackCooldown = 0.3f;
                    return;
                }
                break;
            case AttackDirection::Up:
                if ( float(dy) / distance > float(m_rect.h) / std::sqrt(m_rect.w*m_rect.w + m_rect.h*m_rect.w)){
                    m_attackCooldown = 0.3f;
                    return;
                }
                break;
            case AttackDirection::Down:
                if ( float(dy) / distance < float(m_rect.h) / std::sqrt(m_rect.w*m_rect.w + m_rect.h*m_rect.w)){
                    m_attackCooldown = 0.3f;
                    return;
                }
                break;
        }

    // Normalize direction (THIS enables diagonal)
    dx /= distance;
    dy /= distance;

    // Get current speed magnitude
    float currentVX = ball.getVelocity().x;
    float currentVY = ball.getVelocity().y;
    float currentSpeed = std::sqrt(currentVX * currentVX +
                                   currentVY * currentVY);

    // New speed = old magnitude + attack strength
    float newSpeed = currentSpeed + IMPULSE_STRENGTH;

    if (newSpeed > MAX_SPEED)
        newSpeed = MAX_SPEED;

    // Redirect velocity to attack direction
    ball.getVelocity().x = dx * newSpeed;
    ball.getVelocity().y = dy * newSpeed;

    m_attackCooldown = 0.3f;

    // Set new owner
    ball.SetOwner(this);
}

void Player::Bunt(Ball& ball)
{
    const float BUNT_RADIUS = 80.0f;

    float px = m_rect.x + m_rect.w * 0.5f;
    float py = m_rect.y + m_rect.h * 0.5f;

    SDL_FRect& ballRect = ball.GetRect();
    float bx = ballRect.x + ballRect.w * 0.5f;
    float by = ballRect.y + ballRect.h * 0.5f;

    float dx = bx - px;
    float dy = by - py;

    float distSq = dx * dx + dy * dy;

    if (distSq <= BUNT_RADIUS * BUNT_RADIUS)
    {
        ball.StartBunt(this, m_facing);
    }
}

bool Player::Check_collision(Ball& ball)
{
    // -------------------------------------------------
    // 1. Ownership rules (BALL hitting PLAYER only)
    // -------------------------------------------------
    if (ball.IsNeutral())
        return false;

    if (ball.GetOwner() == this)
        return false;

    const float HURTBOX_WIDTH_RATIO  = 0.6f;
    const float HURTBOX_HEIGHT_RATIO = 0.75f;

    float hurtWidth  = m_rect.w * HURTBOX_WIDTH_RATIO;
    float hurtHeight = m_rect.h * HURTBOX_HEIGHT_RATIO;

    float hurtX = m_rect.x + (m_rect.w - hurtWidth) * 0.5f;
    float hurtY = m_rect.y + (m_rect.h - hurtHeight);

    SDL_FRect hurtbox = { hurtX, hurtY, hurtWidth, hurtHeight };

    SDL_FRect& ballRect = ball.GetRect();

    // -------------------------------------------------
    // 2. AABB collision
    // -------------------------------------------------
    if (RectsIntersect(hurtbox, ballRect))
    {
        m_hp -= BASE_HIT_DAMAGE;
        return true;
    }

    return false;
}