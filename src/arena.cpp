#include "arena.h"

static constexpr float BOUNCE_LOSS = 10.0f;

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
    m_canStep.push_back(false);

    // Bottom wall
    m_walls.push_back({
        0.0f,
        static_cast<float>(m_height - m_wallThickness),
        static_cast<float>(m_width),
        static_cast<float>(m_wallThickness)
    });
    m_canStep.push_back(true);

    // Left wall
    m_walls.push_back({
        0.0f,
        0.0f,
        static_cast<float>(m_wallThickness),
        static_cast<float>(m_height)
    });
    m_canStep.push_back(false);

    // Right wall
    m_walls.push_back({
        static_cast<float>(m_width - m_wallThickness),
        0.0f,
        static_cast<float>(m_wallThickness),
        static_cast<float>(m_height)
    });
    m_canStep.push_back(false);

    if (ver == 1){
        m_walls.push_back({
            static_cast<float>(180),
            static_cast<float>(360),
            static_cast<float>(345),
            static_cast<float>(m_wallThickness)
        });
        m_canStep.push_back(true);
        m_walls.push_back({
            static_cast<float>(765),
            static_cast<float>(360),
            static_cast<float>(345),
            static_cast<float>(m_wallThickness)
        });
        m_canStep.push_back(true);
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
            (ballPos.x - radius < it.x + it.w && ballPos.x + radius > it.x)
            && (ballPos.y - radius < it.y + it.h && ballPos.y + radius > it.y)
        ){
            Vector2 normal;
            float overlapX = std::min(ballRect.x - it.x,
                                      it.x + it.w - ballRect.x);
            float overlapY = std::min(ballRect.y - it.y,
                                      it.y + it.h - ballRect.y);

            if (overlapX < overlapY){
                if (ballPos.x - radius < it.x){
                    //left
                    normal = Vector2(-1.0f, 0.0f);
                    ballRect.x = it.x - ballRect.h;
                }
                else {
                    normal = Vector2(1.0f, 0.0f); 
                    ballRect.x = it.x + it.w;
                }
            }
            else{
                if (ballPos.y - radius < it.y){
                    //top
                    normal = Vector2(0.0f, -1.0f);
                    ballRect.y = it.y - ballRect.h;
                }
                else {
                    normal = Vector2(0.0f, 1.0f);
                    ballRect.y = it.y + it.h;
                }
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

bool Arena::collidePlayer(SDL_FRect &playerRect, Vector2 &playerVel) {
    bool ret = false;

    for (int i = 0; i < m_walls.size(); i++) {
        const SDL_FRect& wall = m_walls[i];

        // AABB overlap check
        if (playerRect.x < wall.x + wall.w && playerRect.x + playerRect.w > wall.x &&
            playerRect.y < wall.y + wall.h && playerRect.y + playerRect.h > wall.y) {

            // Compute overlap depth along X and Y
            float overlapX = std::min(playerRect.x + playerRect.w - wall.x,
                                      wall.x + wall.w - playerRect.x);
            float overlapY = std::min(playerRect.y + playerRect.h - wall.y,
                                      wall.y + wall.h - playerRect.y);

            if (overlapX < overlapY) {
                // Resolve horizontally
                if (playerRect.x < wall.x) {
                    // Colliding from left
                    playerRect.x = wall.x - playerRect.w;
                    if (playerVel.x > 0.0f) playerVel.x = 0.0f;
                } else {
                    // Colliding from right
                    playerRect.x = wall.x + wall.w;
                    if (playerVel.x < 0.0f) playerVel.x = 0.0f;
                }
            } else {
                // Resolve vertically
                if (playerRect.y < wall.y) {
                    // Colliding from top
                    playerRect.y = wall.y - playerRect.h;
                    if (playerVel.y > 0.0f) playerVel.y = 0.0f;
                    ret = ret || m_canStep[i];
                } else {
                    // Colliding from bottom
                    playerRect.y = wall.y + wall.h;
                    if (playerVel.y < 0.0f) playerVel.y = 0.0f;
                }
            }
        }
    }

    return ret;
}


void Arena::CheckCollisionCCD(SDL_FRect& ballRect, Vector2& vel, float dt) const
{
    float radius = ballRect.h * 0.5f;
    Vector2 oldPos(ballRect.x + radius, ballRect.y + radius);
    Vector2 newPos = oldPos.add_N(vel.mul_N(dt));

    for (const auto& wall : m_walls) {
        // Check overlap of swept segment with wall bounds
        // Horizontal walls
        if (vel.y != 0.0f) {
            float wallTop = wall.y;
            float wallBottom = wall.y + wall.h;

            // If moving downward and crossing top of wall
            if (oldPos.y + radius <= wallTop && newPos.y + radius >= wallTop &&
                oldPos.x + radius > wall.x && oldPos.x - radius < wall.x + wall.w) {
                float t = (wallTop - (oldPos.y + radius)) / (newPos.y - oldPos.y);
                Vector2 collisionPoint = oldPos.add_N(newPos.sub_N(oldPos).mul_N(t));

                // Reflect velocity vertically
                Vector2 n(0.0f, -1.0f);
                double angle = vel.angle(n);
                double d_angle = 180.0 -  (180.0 - angle) * 2;
                vel.rotate(d_angle);
                float speed = std::max(vel.length() - BOUNCE_LOSS, 0.0f);
                vel.normalize();
                vel.mul(speed);

                // Advance to collision point, then continue remaining time
                ballRect.x = collisionPoint.x - radius;
                ballRect.y = collisionPoint.y - radius;
                float remaining = (1.0f - t) * dt;
                ballRect.x += vel.x * remaining;
                ballRect.y += vel.y * remaining;
                return; // handled one collision
            }

            // If moving upward and crossing bottom of wall
            if (oldPos.y - radius >= wallBottom && newPos.y - radius <= wallBottom &&
                oldPos.x + radius > wall.x && oldPos.x - radius < wall.x + wall.w) {
                float t = (wallBottom - (oldPos.y - radius)) / (newPos.y - oldPos.y);
                Vector2 collisionPoint = oldPos.add_N((newPos.sub_N(oldPos)).mul_N(t));

                Vector2 n(0.0f, 1.0f);
                double angle = vel.angle(n);
                double d_angle = 180.0 -  (180.0 - angle) * 2;
                vel.rotate(d_angle);
                float speed = std::max(vel.length() - BOUNCE_LOSS, 0.0f);
                vel.normalize();
                vel.mul(speed);

                ballRect.x = collisionPoint.x - radius;
                ballRect.y = collisionPoint.y - radius;
                float remaining = (1.0f - t) * dt;
                ballRect.x += vel.x * remaining;
                ballRect.y += vel.y * remaining;
                return;
            }
        }

        // Vertical walls
        if (vel.x != 0.0f) {
            float wallLeft = wall.x;
            float wallRight = wall.x + wall.w;

            if (oldPos.x + radius <= wallLeft && newPos.x + radius >= wallLeft &&
                oldPos.y + radius > wall.y && oldPos.y - radius < wall.y + wall.h) {
                float t = (wallLeft - (oldPos.x + radius)) / (newPos.x - oldPos.x);
                Vector2 collisionPoint = oldPos.add_N((newPos.sub_N(oldPos)).mul_N(t));

                Vector2 n(-1.0f, 0.0f);
                double angle = vel.angle(n);
                double d_angle = 180.0 -  (180.0 - angle) * 2;
                vel.rotate(d_angle);
                float speed = std::max(vel.length() - BOUNCE_LOSS, 0.0f);
                vel.normalize();
                vel.mul(speed);

                ballRect.x = collisionPoint.x - radius;
                ballRect.y = collisionPoint.y - radius;
                float remaining = (1.0f - t) * dt;
                ballRect.x += vel.x * remaining;
                ballRect.y += vel.y * remaining;
                return;
            }

            if (oldPos.x - radius >= wallRight && newPos.x - radius <= wallRight &&
                oldPos.y + radius > wall.y && oldPos.y - radius < wall.y + wall.h) {
                float t = (wallRight - (oldPos.x - radius)) / (newPos.x - oldPos.x);
                Vector2 collisionPoint = oldPos.add_N((newPos.sub_N(oldPos)).mul_N(t));

                Vector2 n(1.0f, 0.0f);
                double angle = vel.angle(n);
                double d_angle = 180.0 -  (180.0 - angle) * 2;
                vel.rotate(d_angle);
                float speed = std::max(vel.length() - BOUNCE_LOSS, 0.0f);
                vel.normalize();
                vel.mul(speed);

                ballRect.x = collisionPoint.x - radius;
                ballRect.y = collisionPoint.y - radius;
                float remaining = (1.0f - t) * dt;
                ballRect.x += vel.x * remaining;
                ballRect.y += vel.y * remaining;
                return;
            }
        }
    }

    // No collision: just advance normally
    ballRect.x += vel.x * dt;
    ballRect.y += vel.y * dt;
}
void Arena::DrawScoreboard(SDL_Renderer* renderer, TTF_Font* fontScore, int *scoreboard){
    if (!fontScore) return;

    // Màu trắng nhưng cực kỳ mờ (Alpha = 40)
    SDL_Color scoreColor = { 255, 255, 255, 40 }; 

    for (int i = 0; i < 2; ++i) {
        char scoreStr[4];
        snprintf(scoreStr, sizeof(scoreStr), "%d", scoreboard[i]);

        SDL_Surface* surface = TTF_RenderText_Blended(fontScore, scoreStr, 0, scoreColor);
        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            
            float textW = (float)surface->w;
            float textH = (float)surface->h;

            // Vị trí: Số bên trái ở 1/4 chiều rộng, số bên phải ở 3/4 chiều rộng
            float posX = (i == 0) ? (m_width * 0.25f - textW / 2.0f) 
                                 : (m_width * 0.75f - textW / 2.0f);
            float posY = (m_height / 2.0f) - (textH / 2.0f);

            SDL_FRect destRect = { posX, posY, textW, textH };
            
            // Vẽ lên màn hình
            SDL_RenderTexture(renderer, texture, NULL, &destRect);

            // Dọn dẹp ngay để tránh leak
            SDL_DestroySurface(surface);
            SDL_DestroyTexture(texture);
        }
    }
}
