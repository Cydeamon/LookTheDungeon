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

    if (EditorUI::GetInstance().IsMainRenderWindowIsHovered())
    {
        if (Input::IsJustPressed(MouseButton::LEFT))
        {
            levelObjects.push_back(activePlaceableObject);
            activePlaceableObject = nullptr;
        }

        if (Input::IsPressed(MouseButton::RIGHT) && Input::IsJustPressed(SCROLL_UP))
            EditorUI::GetInstance().FloorHeight -= 1;
        if (Input::IsPressed(MouseButton::RIGHT) && Input::IsJustPressed(SCROLL_DOWN))
            EditorUI::GetInstance().FloorHeight += 1;

        if (Input::IsJustPressed(ESCAPE))
        {
            delete activePlaceableObject;
            EditorUI::GetInstance().SelectedAsset = nullptr;
            activePlaceableObject = nullptr;
        }
    }
}

void Editor::handleSelectedAssetPlacement()
{
    isometricCameraMouseRayCast->StopAtY(EditorUI::GetInstance().FloorHeight);
    isometricCamera->PerformMouseRayCast();

    // If asset selected, but active object is of different type - delete it
    if (activePlaceableObject != nullptr && EditorUI::GetInstance().SelectedAsset != nullptr && EditorUI::GetInstance().SelectedAsset->path != activePlaceableObject->GetModelPath())
    {
        delete activePlaceableObject;
        activePlaceableObject = nullptr;
    }

    // If asset selected, but active object is not created - create it
    if (activePlaceableObject == nullptr && EditorUI::GetInstance().SelectedAsset != nullptr)
    {
        activePlaceableObject = new Model(EditorUI::GetInstance().SelectedAsset->path);
    }

    // Adjust active object position
    if (activePlaceableObject != nullptr)
    {
        Vector3 pos = isometricCameraMouseRayCast->GetStopPosition();

        if (EditorUI::GetInstance().StickToGrid)
        {
            pos.x =
                floorf(pos.x / EditorUI::GetInstance().StickyGridSize[0]) *
                EditorUI::GetInstance().StickyGridSize[0] +
                EditorUI::GetInstance().StickyGridOffset[0];

            pos.z =
                floorf(pos.z / EditorUI::GetInstance().StickyGridSize[1]) *
                EditorUI::GetInstance().StickyGridSize[1] +
                EditorUI::GetInstance().StickyGridOffset[1];
        }

        activePlaceableObject->SetPosition(pos);
    }
}

Editor::~Editor() = default;
