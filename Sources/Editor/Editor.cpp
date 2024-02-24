#include "Editor.h"
#include "EditorUI.h"
#include "CMakeConfig.h"
#include "IsometricCamera.h"

/**
 * TODO: Navmeshes
 * TODO: 90deg camera rotation via Num4, Num6
 */

Editor::Editor()
{
    init();
    Engine::GetInstance().SetWindowTitle(PROJECT_LABEL);
}

void Editor::init()
{
    Engine::GetInstance().Init();
    Engine::GetInstance().SetWindowResolution(1600, 900);
    Engine::GetInstance().SetEngineMode(EngineMode::MODE_3D);
    Engine::GetInstance().CenterWindow();
}

void Editor::Run()
{
    IsometricCamera camera;

    int size = 10;
    int tileSize = 4;
    std::vector<std::vector<Model *>> floorTiles(size, std::vector<Model *>(size));

    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            floorTiles[i][j] = new Model("Assets/Models/Dungeon/floor_tile_large.gltf.glb");
            floorTiles[i][j]->SetPosition(i * tileSize, 0, j * tileSize);
        }
    }

    Model *player = new Model("Assets/Models/Characters/Knight.glb");

    while (Engine::GetInstance().IsRunning())
    {
        /***************************************************/
        /********************** Update *********************/
        handleInput();
        Engine::GetInstance().Update();
        update();

        /***************************************************/
        /********************** Render *********************/
        EditorUI::GetInstance().Update();
        Engine::GetInstance().DrawFrame();
    }
}

void Editor::update()
{
    // TODO: Update game logic here
}

void Editor::handleInput()
{
    // TODO: Handle user input here
}

Editor::~Editor() = default;
