#include "Game.h"
#include "CMakeConfig.h"
#include "FreeMoveCamera.h"

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
}

void Game::Run()
{
    FreeMoveCamera camera;
    camera.SetFOV(90);
    camera.SetPositionY(2);
    camera.SetRotation(camera.GetRotation().Rotate(0, 90, 0));

    Model *model1 = new Model("Assets/Models/backpack.obj");

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
    // TODO: Update game logic here
}

void Game::handleInput()
{
    // TODO: Handle user input here
}

Game::~Game() = default;
