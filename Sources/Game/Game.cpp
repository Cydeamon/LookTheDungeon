#include "Game.h"
#include "CMakeConfig.h"
#include "IsometricCamera.h"
#include "UI.h"

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

    int size = 10;
    int tileSize = 4;
    std::vector<std::vector<Model*>> floorTiles(size, std::vector<Model*>(size));

    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            floorTiles[i][j] = new Model("Assets/Models/Dungeon/floor_tile_large.gltf.glb");
            floorTiles[i][j]->SetPosition(i * tileSize, 0, j * tileSize);
        }
    }

    Model *player = new Model("Assets/Models/Characters/Knight.glb");

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

void Game::setupUI()
{
}

Game::~Game() = default;
