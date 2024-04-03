#include "Game.h"
#include "CMakeConfig.h"
#include "IsometricCamera.h"
#include "Editor/Project.h"

Game::Game()
{
    engine = &Engine::GetInstance();
    init();
    engine->SetWindowTitle(PROJECT_LABEL);
}

void Game::init()
{
    engine->Init();
    engine->SetWindowResolution(1600, 900);
    engine->SetEngineMode(EngineMode::MODE_3D);
    engine->CenterWindow();
    setupUI();
}

void Game::Run()
{
    IsometricCamera camera;
    Project project("Levels/level1.lvl");
    levelObjects = project.GetGameObjects();
    char title[64];

    while (engine->IsRunning())
    {
        /***************************************************/
        /********************** Update *********************/
        handleInput();
        engine->Update();
        update();

        sprintf(title, "%s - FPS: %i, Avg. FPS: %i", PROJECT_LABEL, engine->GetFPS(), engine->GetAverageFPS());
        engine->SetWindowTitle(title);

        /***************************************************/
        /********************** Render *********************/
        engine->DrawFrame();
    }
}

void Game::update()
{
    // TODO: Update game logic here
}

void Game::handleInput()
{
    // TODO: Handle user input here
}

void Game::setupUI()
{
}

Game::~Game() = default;
