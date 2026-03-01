#include <SDL3/SDL.h>
#include <iostream>

#include "game.h"
#include "arena.h"
#include "ball.h"
#include "player.h"

int main(int argc, char* argv[])
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
        return -1;
    }

    GameWindow *window = new GameWindow();
    if (window->init("Ricochet test") != 0){
        std::cerr << "Failed to initialize GameWindow: " << SDL_GetError() << std::endl;
        delete(window);
        SDL_Quit();
        return -1;
    }

    // --- Create Game Objects ---
    Arena arena = Arena(1);
    Ball ball(arena.getBallStart(), 20.0f);
    Player player(Vector2(600.0f, 500.0f));
    Player player2(Vector2(400.0f, 500.0f), 2);

    bool running = true;

    Uint64 lastCounter = SDL_GetPerformanceCounter();
    Uint64 frequency = SDL_GetPerformanceFrequency();

    while (running)
    {
        // ---- Events ----
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                running = false;

            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN){
                int x = event.button.x;
                int y = event.button.y;
                printf("%d, %d\n", x, y);
            }

            if (event.type == SDL_EVENT_KEY_DOWN &&
                event.key.repeat == false)
            {
                if (event.key.scancode == SDL_SCANCODE_J)
                {
                    player.PerformAttack(ball);
                }
                else if (event.key.scancode == SDL_SCANCODE_RSHIFT)
                {
                    player2.PerformAttack(ball);
                }
                else if (event.key.scancode == SDL_SCANCODE_K)
                    player.Bunt(ball);

                if (event.key.scancode == SDL_SCANCODE_PERIOD)
                    player2.Bunt(ball);
                
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

        player.HandleInput(keyboardState);
        player2.HandleInput(keyboardState);
        // ---- Update ----
        ball.Update(deltaTime, arena);
        bool hurted = player.Check_collision(ball) or player2.Check_collision(ball);
        if (hurted){
            ball.GetRect().x = arena.getBallStart().x;
            ball.GetRect().y = arena.getBallStart().y;
            ball.setVelocity(Vector2(0.0f, 0.0f));
        }
        arena.CheckCollision(ball.GetRect(), ball.getVelocity());

        player.Update(deltaTime, arena);
        player2.Update(deltaTime, arena);
        // ---- Render ----
        SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 255);
        SDL_RenderClear(window->renderer);

        arena.Render(window->renderer);
        ball.Render(window->renderer);
        player.Render(window->renderer);
        player2.Render(window->renderer);
        SDL_RenderPresent(window->renderer);
    }
    delete(window);
    SDL_Quit();

    return 0;
}