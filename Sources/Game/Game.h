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
    std::vector <GameObject3D *> levelObjects;

    void init();
    void update();
    void handleInput();
    void setupUI();
};