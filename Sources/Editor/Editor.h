#pragma once

#include "../Engine/Engine.h"

class Editor
{
public:
    Editor();
    ~Editor();

    void Run();

private:
    Engine* engine;
    UI* ui;

    void init();
    void update();
    void handleInput();
    void setupUI();
};