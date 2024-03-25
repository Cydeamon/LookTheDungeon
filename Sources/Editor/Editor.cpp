#include "CMakeConfig.h"
#include "Editor.h"
#include "EditorUI.h"
#include "IsometricCamera.h"
#include "FreeMoveCamera.h"
#include <cmath>

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
    Engine::GetInstance().SetWindowIcon("Assets/icon-window.png");
    Engine::GetInstance().Init();
    Engine::GetInstance().SetEngineMode(EngineMode::MODE_3D);

    // Load configuration
    Config::GetInstance().SetIniFileName("LookTheDungeonEditor.ini");
    Config::GetInstance().SetSaveOnValuesChange(true);
    Config::GetInstance().Load();

    // Init window
    int windowWidth;
    int windowHeight;
    int windowPosX = 0;
    int windowPosY = 0;
    bool windowMaximized;

    try {
        windowWidth = Config::GetInstance().GetValue<int>("Window", "Width");
        windowHeight = Config::GetInstance().GetValue<int>("Window", "Height");
        windowPosX = Config::GetInstance().GetValue<int>("Window", "Position X");
        windowPosY = Config::GetInstance().GetValue<int>("Window", "Position Y");
        windowMaximized = Config::GetInstance().GetValue<bool>("Window", "Maximized");
    } catch (std::exception &e) {
        windowWidth = 1280;
        windowHeight = 720;
        windowMaximized = true;

        Config::GetInstance().SetValue("Window", "Width", windowWidth);
        Config::GetInstance().SetValue("Window", "Height", windowHeight);
        Config::GetInstance().SetValue("Window", "Maximized", windowMaximized);
    }

    Engine::GetInstance().SetWindowResolution(windowWidth, windowHeight);

    if (windowMaximized)
        Engine::GetInstance().SetWindowMaximized();

    // Initialize editor
    editorUI = &EditorUI::GetInstance();

    isometricCameraMouseRayCast = new RayCast3D();
    isometricCameraMouseRayCast->EnableDebugDraw(true);

    isometricCamera = new IsometricCamera();
    isometricCamera->SetMouseRayCast(isometricCameraMouseRayCast);

    freeMoveCamera = new FreeMoveCamera();
    freeMoveCamera->SetFOV(60.0f);

    editorUI->AddCamera(isometricCamera);
    editorUI->AddCamera(freeMoveCamera);
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

        if (editorUI->SelectedGameObject)
            (editorUI->SelectedGameObject->GetChildren<Collider3D>()[0])->SetColor(Color::Magenta());

        editorUI->Update();
        Engine::GetInstance().DrawFrame();
    }
}

void Editor::update()
{
    if (editorUI->IsMainRenderWindowIsHovered())
    {
        isometricCameraMouseRayCast->StopAtY(floor);
        handleSelectedAssetPlacement();
    }
}

void Editor::handleInput()
{
    editorUI->UpdateCamerasStates();

    if (editorUI->IsMainRenderWindowIsHovered())
    {
        /*****************************************************************************/
        /*****************  Check buttons input (mouse and keyboard)  ****************/
        /*****************************************************************************/


        /////////////////////////////////////////////////////////////////////////////
        /////   Placing objects with left mouse button

        if (Input::IsJustPressed(MouseButton::LEFT))
        {
            if (editorUI->RotationMode)
            {
                editorUI->RememberTransforms();
                editorUI->RotationMode = false;
                Input::CaptureMouse(false);
            }

            if (editorUI->SelectedGameObject)
            {
                Model *selectedObject = dynamic_cast<Model *>(editorUI->SelectedGameObject);

                if (selectedObject)
                {
                    selectedObject->GetChildren<Collider3D>()[0]->SetDiscoverableByRayCast(true);
                    levelObjects.push_back(selectedObject);
                    editorUI->UpdateSelectedObjectProperties();

                    if (!editorUI->IsEditMode())
                    {
                        selectedObject->GetChildren<Collider3D>()[0]->SetColor(Color::Cyan());
                        editorUI->SelectedGameObject = nullptr;
                    }
                    else
                    {
                        if (hoveredCollider && hoveredCollider->GetParent() != editorUI->SelectedGameObject)
                        {
                            selectedObject->GetChildren<Collider3D>()[0]->SetColor(Color::Cyan());
                            editorUI->SelectedGameObject = hoveredCollider->GetParent();
                        }
                    }
                }

                if (editorUI->IsEditMode())
                    editorUI->MoveWithMouse = false;
            }

            if (hoveredCollider && !editorUI->IsEditMode())
            {
                GameObject *obj = hoveredCollider->GetParent<GameObject>();

                if (obj)
                {
                    editorUI->SelectedGameObject = hoveredCollider->GetParent();
                    editorUI->SetEditMode(true);
                    hoveredCollider = nullptr;
                }
            }
        }


        /////////////////////////////////////////////////////////////////////////////
        /////   Changing floor height

        if (Input::IsPressed(MouseButton::RIGHT) && Input::IsJustPressed(SCROLL_UP))
            editorUI->FloorHeight -= 1;
        if (Input::IsPressed(MouseButton::RIGHT) && Input::IsJustPressed(SCROLL_DOWN))
            editorUI->FloorHeight += 1;


        /////////////////////////////////////////////////////////////////////////////
        /////   Rotating selected object

        if (Input::IsJustPressed(R) && editorUI->IsEditMode() && !editorUI->MoveWithMouse)
        {
            Model *selectedObject = dynamic_cast<Model *>(editorUI->SelectedGameObject);

            if (selectedObject)
            {
                editorUI->RotationMode = !editorUI->RotationMode;

                if (!editorUI->RotationMode)
                {
                    selectedObject->SetRotation(editorUI->PrevObjectRotation);
                    Input::CaptureMouse(false);
                }
                else
                {
                    editorUI->PrevObjectRotation = selectedObject->GetRotation();
                    Input::CaptureMouse(true);
                    rotationModeIntermediate = selectedObject->GetRotation();
                }
            }
        }

        if (editorUI->RotationMode)
        {
            Model *selectedObject = dynamic_cast<Model *>(editorUI->SelectedGameObject);

            if (selectedObject)
            {
                rotationModeIntermediate.SetYaw(rotationModeIntermediate.Yaw() + (Input::GetMouseMovementDelta().X * 0.1f));

                if (Input::IsPressed(ANY_CONTROL))
                {
                    // Lock yaw to 22.5 degrees increments
                    Euler rotation = rotationModeIntermediate;
                    rotation.SetYaw(floorf(rotation.Yaw() / 22.5f) * 22.5f);
                    selectedObject->SetRotation(rotation);
                }
                else
                    selectedObject->SetRotation(rotationModeIntermediate);
            }
            else
            {
                editorUI->RotationMode = false;
                Input::CaptureMouse(false);
            }
        }

        /////////////////////////////////////////////////////////////////////////////
        /////   Deleting objects

        if (Input::IsJustPressed(KeyboardKey::DELETE))
        {
            if (editorUI->IsEditMode())
            {
                erase_if(levelObjects, [](GameObject *obj) { return obj == EditorUI::GetInstance().SelectedGameObject; });
                delete editorUI->SelectedGameObject;
                editorUI->SelectedGameObject = nullptr;
                editorUI->SetEditMode(false);
            }
        }


        /////////////////////////////////////////////////////////////////////////////
        /////   Canceling edits or new object placement

        if (Input::IsJustPressed(ESCAPE))
        {
            if (editorUI->IsEditMode())
                editorUI->SetEditMode(false);
            else
            {
                delete editorUI->SelectedGameObject;
                editorUI->SelectedGameObject = nullptr;
                editorUI->SelectedAsset = nullptr;
            }

            editorUI->MoveWithMouse = false;
        }


        /////////////////////////////////////////////////////////////////////////////
        /////   Moving selected object

        if (Input::IsJustPressed(G) && editorUI->IsEditMode())
        {
            Model *selectedObject = dynamic_cast<Model *>(editorUI->SelectedGameObject);

            if (selectedObject)
                selectedObject->GetChildren<Collider3D>()[0]->SetDiscoverableByRayCast(true);

            if (editorUI->MoveWithMouse)
                editorUI->RestoreTransforms();
            else
                editorUI->RememberTransforms();

            editorUI->MoveWithMouse = !editorUI->MoveWithMouse;
        }



        /*****************************************************************************/
        /***************************  Check mouse hovers  ****************************/
        /*****************************************************************************/

        /////////////////////////////////////////////////////////////////////////////
        /////   Hovering objects with mouse

        if (hoveredCollider)
        {
            hoveredCollider->SetColor(Color::Cyan());
            hoveredCollider = nullptr;
        }

        if (!editorUI->MoveWithMouse)
        {
            if (Collider3D *collider = isometricCameraMouseRayCast->GetCollidedObject())
            {
                if (Collider3D *colliderCube = dynamic_cast<Collider3D *>(collider))
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
    isometricCameraMouseRayCast->StopAtY(editorUI->FloorHeight);
    isometricCamera->PerformMouseRayCast();

    Model *selectedObject = dynamic_cast<Model *>(editorUI->SelectedGameObject);

    if (!editorUI->SelectedGameObject || (selectedObject && editorUI->SelectedGameObject))
    {
        // If asset selected, but active object is not created - create it
        if (selectedObject == nullptr && editorUI->SelectedAsset != nullptr)
        {
            selectedObject = new Model(editorUI->SelectedAsset->path);
            selectedObject->SetRotation(editorUI->PrevObjectRotation);

            // If selected object contains "stairs" in name - generate complex mesh collider
            if (editorUI->SelectedAsset->name.find("stairs") != std::string::npos)
                selectedObject->GenerateMeshCollider();
            else
                selectedObject->GenerateBoxCollider();

            // If model have "Idle" animation - play it
            std::vector<std::string> animations = selectedObject->GetAnimationNames();

            if (std::find(animations.begin(), animations.end(), "Idle") != animations.end())
                selectedObject->PlayAnimation("Idle", AnimationMode::LOOP);

            selectedObject->GetChildren<Collider3D>()[0]->SetDiscoverableByRayCast(false);
            editorUI->SelectedGameObject = selectedObject;
        }

        // Adjust active object position
        if (selectedObject != nullptr && editorUI->MoveWithMouse)
        {
            selectedObject->GetChildren<Collider3D>()[0]->SetDiscoverableByRayCast(false);
            Vector3 pos = isometricCameraMouseRayCast->GetStopPosition();

            if (editorUI->StickToGrid)
            {
                pos.x =
                    floorf(pos.x / editorUI->StickyGridSize[0]) *
                    editorUI->StickyGridSize[0] +
                    editorUI->StickyGridOffset[0];

                pos.z =
                    floorf(pos.z / editorUI->StickyGridSize[1]) *
                    editorUI->StickyGridSize[1] +
                    editorUI->StickyGridOffset[1];
            }

            selectedObject->SetPosition(pos);
        }
    }
}

Editor::~Editor()
{
    Config::GetInstance().SetValue("Window", "Width", Engine::GetInstance().GetWindowSize().X);
    Config::GetInstance().SetValue("Window", "Height", Engine::GetInstance().GetWindowSize().Y);
    Config::GetInstance().SetValue("Window", "Position X", Engine::GetInstance().GetWindowPosition().X);
    Config::GetInstance().SetValue("Window", "Position Y", Engine::GetInstance().GetWindowPosition().Y);
    Config::GetInstance().SetValue("Window", "Maximized", Engine::GetInstance().IsWindowMaximized());
}
