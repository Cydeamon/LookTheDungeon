#pragma once

#include "../Engine/Engine.h"

class Game
{
public:
    Game();
    ~Game();

    void Run();

private:
    Engine* engine;

    void init();
    void update();
    void handleInput();
};