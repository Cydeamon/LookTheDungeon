#include "CMakeConfig.h"
#include "Editor.h"
#include "EditorUI.h"
#include "IsometricCamera.h"
#include "FreeMoveCamera.h"

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
    isometricCameraMouseRayCast = new RayCast3D();
    isometricCameraMouseRayCast->StopAtY(0);

    isometricCamera = new IsometricCamera();
    isometricCameraMouseRayCast->EnableDebugDraw(true);
    isometricCamera->SetMouseRayCast(isometricCameraMouseRayCast);
    
    freeMoveCamera = new FreeMoveCamera();
    freeMoveCamera->SetPosition({0, 3, -3});
    freeMoveCamera->SetRotation({0, 0, 45});

    EditorUI::GetInstance().AddCamera(isometricCamera);
    EditorUI::GetInstance().AddCamera(freeMoveCamera);

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
    if (EditorUI::GetInstance().IsMainRenderWindowIsHovered())
    {
        handleSelectedAssetPlacement();
    }
}

void Editor::handleInput()
{
    EditorUI::GetInstance().UpdateCamerasStates();
}

void Editor::handleSelectedAssetPlacement()
{
    isometricCamera->PerformMouseRayCast();
}

Editor::~Editor() = default;
