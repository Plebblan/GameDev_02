#include <SDL3/SDL.h>
#include <iostream>

#include "arena.h"
#include "ball.h"

int main(int argc, char* argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Ricochet Test",
        1280,
        720,
        0
    );

    if (!window)
    {
        std::cerr << "Window creation failed: " << SDL_GetError() << "\n";
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer)
    {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << "\n";
        return -1;
    }

    // Create arena
    Arena arena;

    // Spawn ball in the center
    Ball ball(640.0f - 10.0f, 360.0f - 10.0f, 20.0f);

    bool running = true;
    Uint64 lastTime = SDL_GetTicks();

    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                running = false;
        }

        // ---- Delta Time ----
        Uint64 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        // ---- Update ----
        ball.Update(deltaTime);

        // Arena handles ricochet
        arena.CheckCollision(ball.GetRect(), ball.GetVelX(), ball.GetVelY());

        // ---- Render ----
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        arena.Render(renderer);
        ball.Render(renderer);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}