#pragma once

#include "../Engine/Engine.h"
#include "FreeMoveCamera.h"
#include "IsometricCamera.h"

class Editor
{
public:
    Editor();
    ~Editor();

    void Run();

private:
    Engine* engine;

    void init();
    void update();
    void handleInput();
    void setupUI();
    IsometricCamera *isometricCamera = nullptr;
    FreeMoveCamera *freeMoveCamera = nullptr;
};