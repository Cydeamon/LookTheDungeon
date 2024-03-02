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

    isometricCamera = new IsometricCamera();
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
        isometricCameraMouseRayCast->StopAtY(floor);
        handleSelectedAssetPlacement();
    }
}

void Editor::handleInput()
{
    EditorUI::GetInstance().UpdateCamerasStates();
}

void Editor::handleSelectedAssetPlacement()
{
    isometricCameraMouseRayCast->StopAtY(EditorUI::GetInstance().GetFloorHeight());
    isometricCamera->PerformMouseRayCast();

    // If asset selected, but active object is of different type - delete it
    if (activePlaceableObject != nullptr && EditorUI::GetInstance().SelectedAsset != nullptr && EditorUI::GetInstance().SelectedAsset->path != activePlaceableObject->GetModelPath())
    {
        std::cout << "Deleting active object" << std::endl;
        delete activePlaceableObject;
        activePlaceableObject = nullptr;
    }

    // If asset selected, but active object is not created - create it
    if (activePlaceableObject == nullptr && EditorUI::GetInstance().SelectedAsset != nullptr)
    {
        std::cout << "Creating new active object: " << EditorUI::GetInstance().SelectedAsset->path << "" << std::endl;
        activePlaceableObject = new Model(EditorUI::GetInstance().SelectedAsset->path);
    }

    // Adjust active object position
    if (activePlaceableObject != nullptr)
    {
        activePlaceableObject->SetPosition(isometricCameraMouseRayCast->GetStopPosition());
    }

    if (Input::IsJustPressed(MouseButton::LEFT))
    {
//        Objects
    }
}

Editor::~Editor() = default;
