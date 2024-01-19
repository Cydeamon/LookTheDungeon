#include "Game.h"
#include "CMakeConfig.h"

Game::Game()
{
    engine = new Engine();
    init();
    engine->SetWindowTitle(PROJECT_LABEL);
}

void Game::init()
{
    engine->Init();
    engine->SetWindowResolution(1280, 720);
    engine->SetInternalResolution(480, 270);
    engine->CenterWindow();
    engine->SetEngineMode(EngineMode::MODE_2D);
    engine->UsePaletteColors(true);
}

void Game::Run()
{
    while (engine->IsRunning())
    {
        /***************************************************/
        /********************** Update *********************/
        handleInput();
        engine->Update();
        update();

        /***************************************************/
        /********************** Render *********************/
        engine->DrawFrame();
    }
}

void Game::update()
{
    // TODO: Update game logic
}

void Game::handleInput()
{
    // TODO: Handle input
}

Game::~Game()
{
    delete engine;
}
