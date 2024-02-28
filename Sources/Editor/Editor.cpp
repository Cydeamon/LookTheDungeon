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
    Engine::GetInstance().SetWindowTitle("Look! The Dungeon! - Level Editor");
}

void Editor::init()
{
    Engine::GetInstance().SetDrawSceneToPrimaryFramebuffer(false);
    Engine::GetInstance().Init();
    Engine::GetInstance().SetWindowResolution(1600, 900);
    Engine::GetInstance().SetEngineMode(EngineMode::MODE_3D);
    Engine::GetInstance().CenterWindow();
}

void Editor::Run()
{
    IsometricCamera camera;

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
