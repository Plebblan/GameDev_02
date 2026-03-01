#include <SDL3/SDL.h>
#include <iostream>

#include "game.h"
#include "arena.h"
#include "ball.h"
#include "player.h"
#include "menu.h"

int main(int argc, char* argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
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

    // --- Menu Selection ---
    Menu menu;
    bool menuActive = true;
    Uint64 lastCounter = SDL_GetPerformanceCounter();
    Uint64 frequency = SDL_GetPerformanceFrequency();
    
    while (menuActive) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                delete(window);
                SDL_Quit();
                return 0;
            }
        }
        
        const bool* keyboardState = SDL_GetKeyboardState(nullptr);
        menu.HandleInput(keyboardState);
        
        if (menu.IsMapSelected()) {
            menuActive = false;
        }
        
        SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 255);
        SDL_RenderClear(window->renderer);
        menu.Render(window->renderer, window->getWidth(), window->getHeight());
        SDL_RenderPresent(window->renderer);
    }

    // --- Create Game Objects ---
    const MapConfig& selectedMap = menu.GetSelectedMap();
    Arena arena(selectedMap.ver, selectedMap.width, selectedMap.height, selectedMap.wallThickness);
    Ball ball(selectedMap.ballSpawnPos, 40.0f);
    Player player(selectedMap.player1SpawnPos);
    Player player2(selectedMap.player2SpawnPos, 2);
    Player clone(selectedMap.player1SpawnPos);
    Player clone2(selectedMap.player2SpawnPos, 2);
    player.setAlly(&clone);
    clone.setAlly(&player);
    player2.setAlly(&clone2);
    clone2.setAlly(&player2);
    clone.kill();
    clone2.kill();
    bool p1_control = true;
    bool p2_control = true;
    int scoreboard[2] = {0};
    TTF_Font* fontScore = TTF_OpenFont("/System/Library/Fonts/Supplemental/Arial.ttf", 300);
    bool running = true;

    lastCounter = SDL_GetPerformanceCounter();
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
                    if (p1_control) player.Bunt(ball);
                    else clone.Bunt(ball);
                }
                else if (event.key.scancode == SDL_SCANCODE_PERIOD)
                {
                    if (p2_control) player2.Bunt(ball);
                    else clone2.Bunt(ball);
                }
                else if (event.key.scancode == SDL_SCANCODE_L)
                {
<<<<<<< HEAD
                    int t = player.CatchThrow(ball);
                }
                else if (event.key.scancode == SDL_SCANCODE_COMMA)
                {
                    int t = player2.CatchThrow(ball);
=======
                    if (p1_control) player.CatchThrow(ball);
                    else clone.CatchThrow(ball);
                }
                else if (event.key.scancode == SDL_SCANCODE_COMMA)
                {
                    if (p2_control) player2.CatchThrow(ball);
                    else clone2.CatchThrow(ball);
                }
            }

            if (event.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN)
            {
                if (event.gbutton.button == SDL_GAMEPAD_BUTTON_SOUTH)
                {
                    player2.PerformAttack(ball);
>>>>>>> 60e8a378d391233235d83004005415d6ae3405b4
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
        if (!clone2.IsDead()) hurted = hurted or clone2.Check_collision(ball);
        if (!clone.IsDead()) hurted = hurted or clone.Check_collision(ball);
        if (hurted){
            ball.GetRect().x = selectedMap.ballSpawnPos.x;
            ball.GetRect().y = selectedMap.ballSpawnPos.y;
            ball.setVelocity(Vector2(0.0f, 0.0f));
            ball.SetOwner(nullptr);
            if (player.IsDead()){
                scoreboard[1] += 1; 
                player.Reset(selectedMap.player1SpawnPos);
                p1_control = true;
                clone.kill();
            }
            else if (player2.IsDead())
            {
                scoreboard[0] += 1;
                player2.Reset(selectedMap.player2SpawnPos);
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