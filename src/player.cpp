#include "player.h"

static constexpr float GRAVITY = 900.0f;
static constexpr float HOLD_GRAVITY = 400.0f;   // Reduced gravity while holding jump
static constexpr float MAX_HOLD_TIME = 0.25f;   // Max time jump can be extended
static constexpr float JUMP_CUT_MULTIPLIER = 0.5f; // Cuts upward velocity if released early
static constexpr int BASE_HIT_DAMAGE = 50; 
static constexpr int BASE_SPEED = 500; 

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
      m_moveSpeed(350.0f),
      m_jumpStrength(600.0f),
      m_doubleJumpStrength(400.0f),
      m_isGrounded(false),
      m_jumpHeld(false),
      m_jumpHoldTime(0.0f),
      m_facing(AttackDirection::Right),
      m_attackCooldown(0.0f),
      m_isAttacking(false),
      m_attackTimer(0.0f),
      m_attackDuration(0.25f),
      m_jumpCount(0),
      m_maxJumps(2),
      m_Noplayer(No),
      m_jumpPressedLastFrame(false),
      m_buntCooldown(0.0f),
      m_isBunting(false),
      m_buntTimer(0.0f),
      m_buntDuration(0.2f),
      m_catchCooldown(0.0f),
      m_isCatching(false),
      m_catchTimer(0.0f),
      m_catchDuration(0.15f)
{
    m_rect = { pos.x, pos.y, 40.0f, 150.0f };
    m_hp = 200;
    m_maxHp = 200; 
    font = TTF_OpenFont("/System/Library/Fonts/Supplemental/Arial.ttf", 20);
    
    if (font) {
        snprintf(pNum, sizeof(pNum), "P%d", m_Noplayer);
        SDL_Color textColor = { 255, 255, 255, 255 };
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, pNum, 0, textColor);
        if (textSurface) {
            m_nameW = (float)textSurface->w;
            m_nameH = (float)textSurface->h;
        }
    }
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

void Player::Update(float deltaTime, Arena arena)
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

    // Cooldown timer
    if (m_buntCooldown > 0.0f)
        m_buntCooldown -= deltaTime;

    // Bunt active window
    if (m_isBunting)
    {
        m_buntTimer -= deltaTime;
        if (m_buntTimer <= 0.0f)
            m_isBunting = false;
    }

    // Cooldown timer
    if (m_catchCooldown > 0.0f)
        m_catchCooldown -= deltaTime;

    // Bunt active window
    if (m_isCatching)
    {
        m_catchTimer -= deltaTime;
        if (m_buntTimer <= 0.0f)
            m_isCatching = false;
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

    bool wasGrounded = m_isGrounded;
    m_isGrounded = arena.collidePlayer(m_rect, m_vel);
    if (m_isGrounded && !wasGrounded)
        {
            m_jumpCount = 0;
            m_jumpHeld = false;
            m_jumpPressedLastFrame = false;
        }
}

void Player::Render(SDL_Renderer* renderer) const
{
    if (m_hp <= 0){
        return;
    }
    float centerX = m_rect.x + m_rect.w / 2.0f;
    float barW = 40.0f;
    float barH = 6.0f;
    float barX = m_rect.x + (m_rect.w / 2.0f) - (barW / 2.0f);
    float barY = m_rect.y - 20.0f;

    // Nền thanh máu (Màu đỏ)
    SDL_FRect healthBg = { barX, barY, barW, barH };
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &healthBg);

    // Lượng máu hiện tại (Màu xanh lá)
    float healthPercent = (float)m_hp / (float)m_maxHp;
    if (healthPercent < 0) healthPercent = 0;
    SDL_FRect healthFill = { barX, barY, barW * healthPercent, barH };
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &healthFill);


    SDL_Color textColor = { 0, 255, 255, 255 };
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, pNum, 0, textColor);
    if (textSurface) {
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        
        // Căn giữa chữ dựa trên m_rect
        float textW = (float)textSurface->w;
        float textH = (float)textSurface->h;
        SDL_FRect textRect = { (m_rect.x + m_rect.w/2.0f) - (textW/2.0f), barY - 18.0f, textW, textH };
        
        SDL_RenderTexture(renderer, textTexture, NULL, &textRect);

        SDL_DestroySurface(textSurface);
        SDL_DestroyTexture(textTexture); // QUAN TRỌNG: Phải destroy nếu tạo trong Render
    }

    if (m_Noplayer == 1)
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    else
        SDL_SetRenderDrawColor(renderer, 0, 120, 255, 255);

    

    SDL_FRect head = { centerX - 15.0f, m_rect.y, 15.0f, 15.0f };
    for (int w = centerX - 15.0f; w <= centerX + 15.0f; w++){
        for (int h = m_rect.y; h <= m_rect.y + 30.0f; h++){
            float dx = w - centerX;
            float dy = h - m_rect.y - 15.0f;
            if (dx * dx + dy * dy <= 15.0f * 15.0f){
                SDL_RenderPoint(renderer, w, h);
            }
        }
    }

    SDL_RenderLine(renderer,
        centerX, m_rect.y,
        centerX, m_rect.y + m_rect.h * 0.5f);

    if (m_isAttacking){
        if (m_facing == AttackDirection::Left){
            SDL_RenderLine(renderer,
            centerX, m_rect.y + 30.0f,
            centerX - 50.0f, m_rect.y + 30.0f);
            SDL_RenderLine(renderer,
            centerX, m_rect.y + 30.0f,
            centerX + 20.0f, m_rect.y + m_rect.h * 0.5f);
        }
        else{
            SDL_RenderLine(renderer,
            centerX, m_rect.y + 30.0f,
            centerX + 50.0f, m_rect.y + 30.0f);
            SDL_RenderLine(renderer,
            centerX, m_rect.y + 30.0f,
            centerX - 20.0f, m_rect.y + m_rect.h * 0.5f);
        }
    }
    else {
        SDL_RenderLine(renderer,
            centerX, m_rect.y + 30.0f,
            centerX - 20.0f, m_rect.y + m_rect.h * 0.5f);
        SDL_RenderLine(renderer,
            centerX, m_rect.y + 30.0f,
            centerX + 20.0f, m_rect.y + m_rect.h * 0.5f);
    }

    SDL_RenderLine(renderer,
        centerX, m_rect.y + m_rect.h * 0.5f,
        m_rect.x, m_rect.y + m_rect.h);

    SDL_RenderLine(renderer,
        centerX, m_rect.y + m_rect.h * 0.5f,
        m_rect.x + m_rect.w, m_rect.y + m_rect.h);

    if (m_isAttacking)
    {
        const int LAYERS = 6;              // how many arcs
        const int SEGMENTS = 28;           // smoothness
        const float BASE_RADIUS = 20.0f;   // inner arc
        float MAX_RADIUS  = 150.0f;  // OUTER ARC = MAX RANGE
        const float ARC_WIDTH = 0.6f;      // arc spread (~35 degrees)

        float centerX = m_rect.x + m_rect.w / 2.0f;
        float centerY = m_rect.y + m_rect.h / 2.0f;

        float baseAngle = 0.0f;

        switch (m_facing)
        {
            case AttackDirection::Right: baseAngle = 0.0f; break;
            case AttackDirection::Left:  baseAngle = PI; break;
            case AttackDirection::Up:    baseAngle = -PI / 2.0f; MAX_RADIUS = 150; break;
            case AttackDirection::Down:  baseAngle = PI / 2.0f; MAX_RADIUS = 150; break;
            default: break;
        }

        float startAngle = baseAngle - ARC_WIDTH;
        float endAngle   = baseAngle + ARC_WIDTH;

        for (int layer = 1; layer <= LAYERS; layer++)
        {
            float t = float(layer) / LAYERS;

            // Radius grows outward
            float radius = BASE_RADIUS + t * (MAX_RADIUS - BASE_RADIUS);

            // Slight alpha fade outward
            Uint8 alpha = static_cast<Uint8>(200 * t + 55);

            if (m_Noplayer == 1)
                SDL_SetRenderDrawColor(renderer, 255, 165, 0, alpha);
            else
                SDL_SetRenderDrawColor(renderer, 255, 0, 255, alpha);

            float step = (endAngle - startAngle) / SEGMENTS;

            for (int i = 0; i < SEGMENTS; ++i)
            {
                float a1 = startAngle + step * i;
                float a2 = startAngle + step * (i + 1);

                float x1 = centerX + cosf(a1) * radius;
                float y1 = centerY + sinf(a1) * radius;

                float x2 = centerX + cosf(a2) * radius;
                float y2 = centerY + sinf(a2) * radius;

                SDL_RenderLine(renderer,
                    static_cast<int>(x1),
                    static_cast<int>(y1),
                    static_cast<int>(x2),
                    static_cast<int>(y2));
            }
        }
    }
    if (m_isBunting)
    {
        if (m_Noplayer == 2)
            SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255); // cyan
        else
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // cyan

        const int layers = 4;
        const float spacing = 22.0f;     // distance between wave layers
        const float lineHalfHeight = 55.0f;  // half height of each line
        const float startOffset = 10.0f; // how far from player it begins

        float centerX = m_rect.x + m_rect.w * 0.5f;
        float centerY = m_rect.y + m_rect.h * 0.5f;

        for (int i = 0; i < layers; ++i)
        {
            float offset = startOffset + i * spacing;

            float x1, y1, x2, y2;

            switch (m_facing)
            {
                case AttackDirection::Right:
                    x1 = centerX + offset;
                    y1 = centerY - lineHalfHeight;
                    x2 = centerX + offset;
                    y2 = centerY + lineHalfHeight;
                    break;

                case AttackDirection::Left:
                    x1 = centerX - offset;
                    y1 = centerY - lineHalfHeight;
                    x2 = centerX - offset;
                    y2 = centerY + lineHalfHeight;
                    break;

                case AttackDirection::Up:
                    x1 = centerX - lineHalfHeight;
                    y1 = centerY - offset;
                    x2 = centerX + lineHalfHeight;
                    y2 = centerY - offset;
                    break;

                case AttackDirection::Down:
                    x1 = centerX - lineHalfHeight;
                    y1 = centerY + offset;
                    x2 = centerX + lineHalfHeight;
                    y2 = centerY + offset;
                    break;

                default:
                    continue;
            }

            SDL_RenderLine(renderer, x1, y1, x2, y2);
        }
    }
}

void Player::PerformAttack(Ball& ball)
{
    if (ball.IsCaught())
        return;
    if (m_attackCooldown > 0.0f )
        return;

    m_isAttacking = true;
    m_attackTimer = m_attackDuration;

    const float IMPULSE_STRENGTH = 400.0f;

    // --- Player center (real center for direction)
    float realPx = m_rect.x + m_rect.w * 0.5f;
    float realPy = m_rect.y + m_rect.h * 0.5f;

    // --- Ball rect
    SDL_FRect& ballRect = ball.GetRect();

    const float HITBOX_WIDTH  = 150.0f;
    const float HITBOX_HEIGHT = 160.0f;
    const float OFFSET = 20.0f;

    SDL_FRect hitbox;

    switch (m_facing)
    {
        case AttackDirection::Right:
            hitbox = { realPx + OFFSET,
                       realPy - HITBOX_HEIGHT * 0.5f,
                       HITBOX_WIDTH,
                       HITBOX_HEIGHT };
            break;

        case AttackDirection::Left:
            hitbox = { realPx - OFFSET - HITBOX_WIDTH,
                       realPy - HITBOX_HEIGHT * 0.5f,
                       HITBOX_WIDTH,
                       HITBOX_HEIGHT };
            break;

        case AttackDirection::Up:
            hitbox = { realPx - HITBOX_HEIGHT * 0.5f,
                       realPy - OFFSET - HITBOX_WIDTH,
                       HITBOX_HEIGHT,
                       HITBOX_WIDTH };
            break;

        case AttackDirection::Down:
            hitbox = { realPx - HITBOX_HEIGHT * 0.5f,
                       realPy + OFFSET,
                       HITBOX_HEIGHT,
                       HITBOX_WIDTH };
            break;

        default:
            return;
    }

    // ACTUAL HIT CHECK
    if (!SDL_HasRectIntersectionFloat(&hitbox, &ballRect))
    {
        m_attackCooldown = 0.3f;
        return;
    }

    // --- Ball center
    float bx = ballRect.x + ballRect.w * 0.5f;
    float by = ballRect.y + ballRect.h * 0.5f;

    // --- Direction based on real player center
    float dx = bx - realPx;
    float dy = by - realPy;

    float distance = std::sqrt(dx * dx + dy * dy);
    if (distance < 0.01f)
        return;

    dx /= distance;
    dy /= distance;

    ball.UnBunt();

    // Preserve magnitude + add impulse
    float currentVX = ball.getVelocity().x;
    float currentVY = ball.getVelocity().y;
    float currentSpeed = std::sqrt(currentVX * currentVX +
                                   currentVY * currentVY);

    float newSpeed = currentSpeed + IMPULSE_STRENGTH;

    ball.getVelocity().x = dx * newSpeed;
    ball.getVelocity().y = dy * newSpeed;

    ball.SetOwner(this);

    m_attackCooldown = 0.4f;
}

void Player::Bunt(Ball& ball)
{
    if (ball.IsCaught())
        return;
    if (m_buntCooldown > 0.0f)
        return;
    
    m_isBunting = true;
    m_buntTimer = m_buntDuration;

    const float HITBOX_WIDTH  = 110.0f;
    const float HITBOX_HEIGHT = 130.0f;
    const float OFFSET = 10.0f;

    float centerX = m_rect.x + m_rect.w * 0.5f;
    float centerY = m_rect.y + m_rect.h * 0.5f;

    SDL_FRect hitbox;

    switch (m_facing)
    {
        case AttackDirection::Right:
            hitbox = { centerX + OFFSET,
                       centerY - HITBOX_HEIGHT * 0.5f,
                       HITBOX_WIDTH,
                       HITBOX_HEIGHT };
            break;

        case AttackDirection::Left:
            hitbox = { centerX - OFFSET - HITBOX_WIDTH,
                       centerY - HITBOX_HEIGHT * 0.5f,
                       HITBOX_WIDTH,
                       HITBOX_HEIGHT };
            break;

        case AttackDirection::Up:
            hitbox = { centerX - HITBOX_HEIGHT * 0.5f,
                       centerY - OFFSET - HITBOX_WIDTH,
                       HITBOX_HEIGHT,
                       HITBOX_WIDTH };
            break;

        case AttackDirection::Down:
            hitbox = { centerX - HITBOX_HEIGHT * 0.5f,
                       centerY + OFFSET,
                       HITBOX_HEIGHT,
                       HITBOX_WIDTH };
            break;

        default:
            return;
    }

    SDL_FRect& ballRect = ball.GetRect();

    if (SDL_HasRectIntersectionFloat(&hitbox, &ballRect))
    {
        ball.StartBunt(this, m_facing);
    }

    m_buntCooldown = 0.2f;
}

void Player::CatchThrow(Ball& ball)
{
    // ------------------------------------
    // CASE 1: Ball is already caught
    // ------------------------------------
    if (ball.IsCaught())
    {
        // Only holder can throw
        if (ball.GetHolder() == this)
        {
            ball.Throw(m_facing);
        }
        return;
    }

    // ------------------------------------
    // CASE 2: Ball is free → attempt catch
    // ------------------------------------

    if (m_catchCooldown > 0.0f)
        return;
    
    m_isCatching = true;
    m_catchTimer = m_catchDuration;

    SDL_FRect& ballRect = ball.GetRect();

    const float CATCH_WIDTH  = 100.0f;
    const float CATCH_HEIGHT = 110.0f;

    float centerX = m_rect.x + m_rect.w * 0.5f;
    float centerY = m_rect.y + m_rect.h * 0.5f;

    SDL_FRect catchBox = {
        centerX - CATCH_WIDTH * 0.5f,
        centerY - CATCH_HEIGHT * 0.5f,
        CATCH_WIDTH,
        CATCH_HEIGHT
    };

    if (SDL_HasRectIntersectionFloat(&catchBox, &ballRect))
    {
        ball.Catch(this);
    }

    m_catchCooldown = 0.9f;
}

SDL_FRect& Player::GetRect()
{
    return m_rect;
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

 
    float currentVX = ball.getVelocity().x;
    float currentVY = ball.getVelocity().y;
    float currentSpeed = std::sqrt(currentVX * currentVX +
                                   currentVY * currentVY);

    if (RectsIntersect(hurtbox, ballRect))
    {
        m_hp -= BASE_HIT_DAMAGE * (currentSpeed/BASE_SPEED);
        return true;
    }

    return false;
}
bool Player::IsDead(){
    return m_hp <= 0;
}
void Player::Reset(Vector2 vec){
    m_rect = { vec.x, vec.y, m_rect.w, m_rect.h };
    m_hp = 200;
    m_cloneUsed = false;
}
Player::~Player()
{
    // Giải phóng Font nếu nó đã được mở
    if (font != nullptr)
    {
        TTF_CloseFont(font);
        font = nullptr;
    }
}

void Player::kill(){
    m_hp = 0;
}

Vector2 Player::getPos(){
    return Vector2(m_rect.x, m_rect.y);
}

bool Player::cloneReady(){
    if (m_cloneUsed) return false;
    m_cloneUsed = true;
    return m_cloneUsed;
}