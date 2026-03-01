#include <SDL3/SDL.h>
#include <iostream>

#include "game.h"
#include "arena.h"
#include "ball.h"
#include "player.h"

int main(int argc, char* argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD) < 0)
    {
        std::cout << "SDL Init failed: " << SDL_GetError() << "\n";
        return -1;
    }
    if (!TTF_Init()) {
        SDL_Log("TTF could not initialize! SDL_ttf Error: %s", SDL_GetError());
    }

    GameWindow *window = new GameWindow();
    if (window->init("Ricochet test") != 0){
        std::cerr << "Failed to initialize GameWindow: " << SDL_GetError() << std::endl;
        delete(window);
        SDL_Quit();
        return -1;
    }

    // --- Create Game Objects ---
    Arena arena;
    Ball ball(arena.getBallStart(), 40.0f);
    Player player(Vector2(600.0f, 500.0f));
    Player player2(Vector2(400.0f, 500.0f), 2);
    Player clone(Vector2(600.0f, 500.0f));
    Player clone2(Vector2(400.0f, 500.0f), 2);
    clone.kill();
    clone2.kill();
    bool p1_control = true;
    bool p2_control = true;
    int scoreboard[2] = {0};
    TTF_Font* fontScore = TTF_OpenFont("/System/Library/Fonts/Supplemental/Arial.ttf", 300);
    bool running = true;

    Uint64 lastCounter = SDL_GetPerformanceCounter();
    Uint64 frequency = SDL_GetPerformanceFrequency();
    bool first = true;
    while (running)
    {
        // ---- Events ----
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                running = false;

            if (event.type == SDL_EVENT_KEY_DOWN &&
                event.key.repeat == false)
            {
                if (event.key.scancode == SDL_SCANCODE_J)
                {
                    if (p1_control)
                    player.PerformAttack(ball);
                    else clone.PerformAttack(ball);
                }
                else if (event.key.scancode == SDL_SCANCODE_RSHIFT)
                {
                    if (p2_control)
                    player2.PerformAttack(ball);
                    else clone2.PerformAttack(ball);
                }
                else if (event.key.scancode == SDL_SCANCODE_E){
                    if (!clone.IsDead()) p1_control = !p1_control;
                }
                else if (event.key.scancode == SDL_SCANCODE_Q){
                    if (player.cloneReady()) {
                        clone.Reset(player.getPos());
                    }
                }
                else if (event.key.scancode == SDL_SCANCODE_COMMA){
                    if (player2.cloneReady()){
                        clone2.Reset(player2.getPos());
                    }
                }
                else if (event.key.scancode == SDL_SCANCODE_SEMICOLON){
                    if (!clone2.IsDead()) p2_control = !p2_control;
                }
                else if (event.key.scancode == SDL_SCANCODE_K)
                {
                    player.Bunt(ball);
                }
                else if (event.key.scancode == SDL_SCANCODE_PERIOD)
                {
                    player2.Bunt(ball);
                }
                else if (event.key.scancode == SDL_SCANCODE_L)
                {
                    player.CatchThrow(ball);
                }
                else if (event.key.scancode == SDL_SCANCODE_COMMA)
                {
                    player2.CatchThrow(ball);
                }
            }

            if (event.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN)
            {
                if (event.gbutton.button == SDL_GAMEPAD_BUTTON_SOUTH)
                {
                    player2.PerformAttack(ball);
                }
            }
        }

        // ---- Delta Time (High Precision) ----
        Uint64 currentCounter = SDL_GetPerformanceCounter();
        float deltaTime = (float)(currentCounter - lastCounter) / frequency;
        lastCounter = currentCounter;

        // Optional safety clamp (prevents physics explosion if debugging)
        if (deltaTime > 0.016f)
            deltaTime = 0.016f;

        // ---- Input ----
        const bool* keyboardState = SDL_GetKeyboardState(nullptr);
        bool hurted = player.Check_collision(ball) or player2.Check_collision(ball);
        if (!clone2.IsDead() and ball.GetOwner() != &player2) hurted = hurted or clone2.Check_collision(ball);
        if (!clone.IsDead() and ball.GetOwner() != &player) hurted = hurted or clone.Check_collision(ball);
        if (hurted){
            ball.GetRect().x = arena.getBallStart().x;
            ball.GetRect().y = arena.getBallStart().y;
            ball.setVelocity(Vector2(0.0f, 0.0f));
            ball.SetOwner(nullptr);
            if (player.IsDead()){
                scoreboard[1] += 1; 
                player.Reset(Vector2(600.0f, 500.0f));
                p1_control = true;
                clone.kill();
            }
            else if (player2.IsDead())
            {
                scoreboard[0] += 1;
                player2.Reset(Vector2(400.0f, 500.0f));
                p2_control = true;
                clone2.kill();
            }
        }
        if (p1_control) player.HandleInput(keyboardState);
        else clone.HandleInput(keyboardState);
        if (p2_control) player2.HandleInput(keyboardState);
        else clone2.HandleInput(keyboardState);
        // ---- Update ----
        ball.Update(deltaTime, arena);
        arena.CheckCollision(ball.GetRect(), ball.getVelocity());

        player.Update(deltaTime, arena);
        player2.Update(deltaTime, arena);
        clone.Update(deltaTime, arena);
        clone2.Update(deltaTime, arena);
        // ---- Render ----
        SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 255);
        SDL_RenderClear(window->renderer);
        arena.DrawScoreboard(window->renderer, fontScore, scoreboard);
        arena.Render(window->renderer);
        ball.Render(window->renderer);
        player.Render(window->renderer);
        player2.Render(window->renderer);
        clone.Render(window->renderer);
        clone2.Render(window->renderer);
        SDL_RenderPresent(window->renderer);
    }
    delete(window);
    SDL_Quit();

    return 0;
}