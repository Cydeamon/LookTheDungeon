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
    // Initialize engine
    Engine::GetInstance().SetDrawSceneToPrimaryFramebuffer(false);
    Engine::GetInstance().Init();
    Engine::GetInstance().SetEngineMode(EngineMode::MODE_3D);
    Engine::GetInstance().SetWindowMaximized();

    // Load configuration
    Config::GetInstance().SetIniFileName("LookTheDungeonEditor.ini");
    Config::GetInstance().SetSaveOnValuesChange(true);
    Config::GetInstance().Load();

    // Initialize editor
    isometricCameraMouseRayCast = new RayCast3D();
    isometricCameraMouseRayCast->EnableDebugDraw(true);

    isometricCamera = new IsometricCamera();
    isometricCamera->SetMouseRayCast(isometricCameraMouseRayCast);

    freeMoveCamera = new FreeMoveCamera();
    freeMoveCamera->SetPosition({0, 3, -3});
    freeMoveCamera->SetRotation({0, 0, 45});
    freeMoveCamera->SetFOV(60.0f);

    EditorUI::GetInstance().AddCamera(isometricCamera);
    EditorUI::GetInstance().AddCamera(freeMoveCamera);
}

void Editor::Run()
{
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
        /*****************************************************************************/
        /*****************  Check buttons input (mouse and keyboard)  ****************/
        /*****************************************************************************/

        if (Input::IsJustPressed(MouseButton::LEFT))
        {
            if (EditorUI::GetInstance().SelectedGameObject)
            {
                Model *selectedObject = dynamic_cast<Model *>(EditorUI::GetInstance().SelectedGameObject);

                if (selectedObject)
                {
                    selectedObject->GetChildren<Collider3D>()[0]->SetDiscoverableByRayCast(true);
                    levelObjects.push_back(selectedObject);
                    EditorUI::GetInstance().UpdateSelectedObjectProperties();

                    if (!EditorUI::GetInstance().IsEditMode())
                    {
                        selectedObject->GetChildren<Cube>()[0]->SetColor(Color::Cyan());
                        EditorUI::GetInstance().SelectedGameObject = nullptr;
                    }
                }

                if (EditorUI::GetInstance().IsEditMode())
                    EditorUI::GetInstance().MoveWithMouse = false;
            }

            if (hoveredCollider && !EditorUI::GetInstance().IsEditMode())
            {
                GameObject *obj = hoveredCollider->GetParent();

                if (obj)
                {
                    EditorUI::GetInstance().SelectedGameObject = hoveredCollider->GetParent();
                    EditorUI::GetInstance().SetEditMode(true);
                    hoveredCollider = nullptr;
                }
            }
        }

        if (Input::IsPressed(MouseButton::RIGHT) && Input::IsJustPressed(SCROLL_UP))
            EditorUI::GetInstance().FloorHeight -= 1;
        if (Input::IsPressed(MouseButton::RIGHT) && Input::IsJustPressed(SCROLL_DOWN))
            EditorUI::GetInstance().FloorHeight += 1;

        if (Input::IsJustPressed(DELETE))
        {
            if (EditorUI::GetInstance().IsEditMode())
            {
                erase_if(levelObjects, [](GameObject *obj) { return obj == EditorUI::GetInstance().SelectedGameObject; });
                delete EditorUI::GetInstance().SelectedGameObject;
                EditorUI::GetInstance().SelectedGameObject = nullptr;
                EditorUI::GetInstance().SetEditMode(false);
            }
        }

        if (Input::IsJustPressed(ESCAPE))
        {
            if (EditorUI::GetInstance().IsEditMode())
                EditorUI::GetInstance().SetEditMode(false);
            else
            {
                delete EditorUI::GetInstance().SelectedGameObject;
                EditorUI::GetInstance().SelectedGameObject = nullptr;
                EditorUI::GetInstance().SelectedAsset = nullptr;
            }

            EditorUI::GetInstance().MoveWithMouse = false;
        }

        if (Input::IsJustPressed(G) && EditorUI::GetInstance().IsEditMode())
        {
            Model *selectedObject = dynamic_cast<Model *>(EditorUI::GetInstance().SelectedGameObject);

            if (selectedObject)
                selectedObject->GetChildren<Collider3D>()[0]->SetDiscoverableByRayCast(true);

            if (EditorUI::GetInstance().MoveWithMouse)
                EditorUI::GetInstance().RestoreTransforms();
            else
                EditorUI::GetInstance().RememberTransforms();

            EditorUI::GetInstance().MoveWithMouse = !EditorUI::GetInstance().MoveWithMouse;
        }

        /*****************************************************************************/
        /***************************  Check mouse hovers  ****************************/
        /*****************************************************************************/

        if (hoveredCollider )
        {
            if (hoveredCollider != EditorUI::GetInstance().SelectedGameObject)
                hoveredCollider->SetColor(Color::Cyan());

            hoveredCollider = nullptr;
        }

        if (!EditorUI::GetInstance().IsEditMode() && !EditorUI::GetInstance().MoveWithMouse)
        {
            if (Collider3D *collider = isometricCameraMouseRayCast->GetCollidedObject())
            {
                if (ColliderCube *colliderCube = dynamic_cast<ColliderCube *>(collider))
                {
                    colliderCube->SetColor(Color::Orange());
                    hoveredCollider = colliderCube;
                }
            }
        }
    }
}

void Editor::handleSelectedAssetPlacement()
{
    isometricCameraMouseRayCast->StopAtY(EditorUI::GetInstance().FloorHeight);
    isometricCamera->PerformMouseRayCast();

    Model *selectedObject = dynamic_cast<Model *>(EditorUI::GetInstance().SelectedGameObject);

    if (!EditorUI::GetInstance().SelectedGameObject || (selectedObject && EditorUI::GetInstance().SelectedGameObject))
    {
        // If asset selected, but active object is not created - create it
        if (selectedObject == nullptr && EditorUI::GetInstance().SelectedAsset != nullptr)
        {
            selectedObject = new Model(EditorUI::GetInstance().SelectedAsset->path);
            selectedObject->SetRotation(EditorUI::GetInstance().PrevObjectRotation);
            selectedObject->GenerateBoxCollider();
            selectedObject->GetChildren<Cube>()[0]->SetColor(Color::Magenta());
            selectedObject->GetChildren<Collider3D>()[0]->SetDiscoverableByRayCast(false);
            EditorUI::GetInstance().SelectedGameObject = selectedObject;
        }

        // Adjust active object position
        if (selectedObject != nullptr && EditorUI::GetInstance().MoveWithMouse)
        {
            selectedObject->GetChildren<Collider3D>()[0]->SetDiscoverableByRayCast(false);
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

            selectedObject->SetPosition(pos);
        }
    }
}

Editor::~Editor() = default;
