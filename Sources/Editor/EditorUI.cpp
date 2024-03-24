#include "EditorUI.h"

EditorUI::EditorUI()
{
    ImGui::LoadIniSettingsFromDisk("EditorUI.ini");
    ImGui::GetIO().IniFilename = "EditorUI.ini";
    ImGui::StyleColorsDark();
    Engine::GetInstance().AddFrameBuffers(2);

    assetPlaceholderTexture = new Texture("Assets/tempModelPreview.jpg");
    fillAssets("Assets/Models/LevelParts", &levelParts);
    fillAssets("Assets/Models/Characters", &characters);

    renderHovered.resize(2);
    gameObjects = Engine::GetInstance().GetGameObjects();

    readConfig();
}

EditorUI::~EditorUI() = default;

EditorUI &EditorUI::GetInstance()
{
    static EditorUI instance;
    return instance;
}

void EditorUI::Update()
{
    SetupLayout();
}

void EditorUI::SetupLayout()
{
    if (!Engine::GetInstance().IsRunning())
        return;

    // Setup docking space
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
    {
        // Setup menu bar
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New"))
                {
                    NotImplementedWarning();
                }

                if (ImGui::MenuItem("Open"))
                {
                    NotImplementedWarning();
                }

                if (ImGui::MenuItem("Save"))
                {
                    NotImplementedWarning();
                }

                if (ImGui::MenuItem("Exit"))
                {
                    exit(0);
                }

                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        // Setup editor panel
        ImGui::Begin("Characters");
        {
            int assetsTablesColumnsCount = (int) (ImGui::GetContentRegionAvail().x / 110);
            if (assetsTablesColumnsCount > 0)
            {
                if (ImGui::BeginTable("Characters Table", assetsTablesColumnsCount))
                {
                    for (Asset &character: characters)
                    {
                        DrawAsset(character);
                        ImGui::TableNextColumn();
                    }

                    ImGui::EndTable();
                }
            }
        }
        ImGui::End();

        ImGui::Begin("Level Assets");
        {
            int assetsTablesColumnsCount = (int) (ImGui::GetContentRegionAvail().x / 110);
            if (assetsTablesColumnsCount > 0)
            {
                if (ImGui::BeginTable("Level Assets Table", assetsTablesColumnsCount))
                {
                    for (Asset &levelPart: levelParts)
                    {
                        DrawAsset(levelPart);
                        ImGui::TableNextColumn();
                    }

                    ImGui::EndTable();
                }
            }
        }
        ImGui::End();

        ImGui::Begin("Editor Settings");
        {
            ImGui::Checkbox("Draw grid", &ShowGrid);
            ImGui::SameLine();
            ImGui::Checkbox("Draw axis", &ShowAxis);
            ImGui::SameLine();
            ImGui::Checkbox("Stick to grid", &StickToGrid);

            ImGui::Checkbox("Draw collision shapes", &DrawCollisionShapes);

            ImGui::Text("Sticky grid size");
            ImGui::SameLine();
            ImGui::InputFloat2("##gridSize", &StickyGridSize[0]);

            ImGui::Text("Sticky grid offset");
            ImGui::SameLine();
            ImGui::InputFloat2("##gridOffset", &StickyGridOffset[0]);

            ImGui::Text("Floor height");
            ImGui::SameLine();
            ImGui::InputFloat("##floorHeight", &FloorHeight);

            if (ShowGrid != prevShowGrid)
            {
                Engine::GetInstance().SetDrawGrid(ShowGrid);
                Config::GetInstance().SetValue("EditorUI", "ShowGrid", ShowGrid);
                prevShowGrid = ShowGrid;
            }

            if (ShowAxis != prevShowAxis)
            {
                Engine::GetInstance().SetDrawAxisLines(ShowAxis);
                Config::GetInstance().SetValue("EditorUI", "ShowAxis", ShowAxis);
                prevShowAxis = ShowAxis;
            }

            if (DrawCollisionShapes != prevDrawCollisionShapes)
            {
                Config::GetInstance().SetValue("EditorUI", "DrawCollisionShapes", DrawCollisionShapes);
                prevDrawCollisionShapes = DrawCollisionShapes;
            }
        }
        ImGui::End();

        ImGui::Begin("Selected Object");
        {
            UpdateSelectedObjectProperties();
            ImGuiInputTextFlags flags = 0;

            if (SelectedGameObject == nullptr)
                flags |= ImGuiInputTextFlags_ReadOnly;

            // Object name
            ImGui::Text("Selected Object: %s", selectedObjectName.c_str());

            // Object transforms
            if (ImGui::InputFloat3("Position", &selectedObjectPosition[0], "%.3f", flags))
                applySelectedObjectsEdits();

            if (ImGui::InputFloat3("Rotation", &selectedObjectRotation[0], "%.3f", flags))
                applySelectedObjectsEdits();

            // Animation
            Model *model = dynamic_cast<Model *>(SelectedGameObject);

            if (model)
            {
                ImGui::NewLine();
                ModelAnimation *animation = model->GetCurrentAnimation();
                ImGui::Text("Animation: %s", (animation ? animation->GetName().c_str() : "None"));

                float time = model->GetCurrentAnimationTime();

                if (ImGui::BeginCombo("##Animations", (animation ? animation->GetName().c_str() : "Not selected")))
                {
                    for (std::string &name: model->GetAnimationNames())
                    {
                        if (ImGui::Selectable(name.c_str()))
                        {
                            model->StopAnimation();
                            model->PlayAnimation(name, AnimationMode::LOOP);
                            animation = model->GetCurrentAnimation();
                        }
                    }

                    ImGui::EndCombo();
                }

                if (animation)
                {
                    if (ImGui::SliderFloat("##Time", &time, 0.0f, animation->GetDuration()))
                    {
                        model->SetCurrentAnimationTime(time);
                    }

                    if (ImGui::Button("Play"))
                    {
                        model->PlayAnimation(animation->GetName(), AnimationMode::LOOP);
                    }

                    ImGui::SameLine();

                    if (ImGui::Button("Pause"))
                    {
                        model->PauseAnimation();
                    }

                    ImGui::SameLine();

                    if (ImGui::Button("Stop"))
                    {
                        model->StopAnimation();
                    }

                    ImGui::SameLine();
                    ImGui::Text("%s / %s", model->GetCurrentAnimationTimeString().c_str(), animation->GetDurationString().c_str());
                }
            }
        }
        ImGui::End();

        ImGui::Begin("Statistics");
        {
            ImGui::Text("FPS: %i", Engine::GetInstance().GetFPS());
            ImGui::Text("Average FPS: %i", Engine::GetInstance().GetAverageFPS());
            ImGui::Text("Elements: %i", Engine::GetInstance().GetGameObjectsCount());
        }
        ImGui::End();

        // Tree view
        ImGui::Begin("Hierarchy");
        {
            drawGameObjectsTree();
        }
        ImGui::End();

        // Tutorial window
        ImGui::Begin("Tutorial");
        {
            if (RotationMode)
            {
                ImGui::Text(
                    "[Esc]         Cancel rotation\n"
                    "[Mouse]       Rotate object\n"
                    "[CTRL+Mouse]  Rotate object with 22.5deg increments\n"
                );
            }
            else if (editMode)
            {
                ImGui::Text(
                    "[G]           Move object\n"
                    "[R]           Rotate object\n"
                    "[LMB]         Apply new position\n"
                    "[RMB + Wheel] Change floor height\n"
                    "[Del]         Delete selected object\n"
                    "[Esc]         Cancel edits\n"
                );
            }
            else if (!SelectedGameObject)
            {
                ImGui::Text(
                    "[LMB]         Select object for editing\n"
                    "[RMB + Wheel] Change floor height\n"
                );
            }
            else
            {
                ImGui::Text(
                    "[LMB]         Apply new position\n"
                    "[RMB + Wheel] Change floor height\n"
                    "[Esc]         Cancel edits\n"
                );
            }
        }
        ImGui::End();

        // Render window
        cameras[0]->SetMainCamera(true);
        ImGui::Begin("Render");
        {
            Engine::GetInstance().DrawFramebufferToImGuiWindow(0);
            renderHovered[0] = ImGui::IsWindowHovered();

            if (ImGui::IsWindowHovered())
            {
                ImGui::GetIO().WantCaptureMouse = false;
                ImGui::GetIO().WantCaptureKeyboard = false;
            }
        }
        ImGui::End();

        // Render window
        cameras[1]->SetMainCamera(true);
        ImGui::Begin("Render2");
        {
            Engine::GetInstance().DrawFramebufferToImGuiWindow(1);
            renderHovered[1] = ImGui::IsWindowHovered();

            if (ImGui::IsWindowHovered())
            {
                ImGui::GetIO().WantCaptureMouse = false;
                ImGui::GetIO().WantCaptureKeyboard = false;
            }
        }
        ImGui::End();
    }
}

void EditorUI::NotImplementedWarning()
{
    EXPECT_ERROR(1, "This functionality is not implemented");
}

void EditorUI::fillAssets(const std::string &path, std::vector<Asset> *assets)
{
    std::string category = path.substr(path.find_last_of('/') + 1);

    for (const auto &entry: std::filesystem::directory_iterator(path))
    {
        if (entry.path().extension() == ".glb")
        {
            Asset asset;
            asset.path = path + "/" + entry.path().filename().string();
            asset.name = entry.path().filename().string();

            int replacePos = asset.name.find(".gltf.glb");
            if (replacePos < asset.name.size())
                asset.name = asset.name.replace(replacePos, 9, "");
            else
            {
                replacePos = asset.name.find(".glb");
                asset.name = asset.name.replace(replacePos, 5, "");
            }

            asset.texturePath = "Assets/ModelsPreviews/" + category + "/" + asset.name + ".png";
            assets->push_back(asset);
        }
    }
}

void EditorUI::DrawAsset(EditorUI::Asset &asset)
{
    ImGui::BeginGroup();
    {
        if (SelectedAsset == &asset)
        {
            const ImVec2 &pos = ImGui::GetCursorScreenPos();
            ImGui::GetWindowDrawList()->AddRectFilled(
                {pos.x - 2, pos.y},
                {pos.x + ImGui::GetContentRegionAvail().x + 8, pos.y + 70},
                IM_COL32(0, 100, 255, 100)
            );
        }

        if (ImGui::IsRectVisible({100, 100}))
        {
            if (asset.texture == nullptr)
                asset.texture = new Texture(asset.texturePath);
        }
        else
        {
            if (asset.texture != nullptr)
            {
                asset.texture->Destroy();
                delete asset.texture;
                asset.texture = nullptr;
            }
        }

        // Center image
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetContentRegionAvail().x - 100) / 2);

        if (asset.texture == nullptr)
            ImGui::Image((void *) assetPlaceholderTexture->GetID(), {100, 100});
        else
            ImGui::Image((void *) asset.texture->GetID(), {100, 100});

        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::Text("%s", asset.name.c_str());
            ImGui::EndTooltip();
        }

        if (ImGui::IsItemClicked())
        {
            SelectedAsset = &asset;
            MoveWithMouse = true;

            if (SelectedGameObject)
            {
                if (!IsEditMode())
                {
                    delete SelectedGameObject;
                    SelectedGameObject = nullptr;
                }

                if (SelectedGameObject)
                {
                    Model *model = dynamic_cast<Model*>(SelectedGameObject);
                    SelectedGameObject = nullptr;

                    if (model)
                    {
                        model->GetChildren<Collider3D>()[0]->SetColor(Color::Cyan());
                        model->GetChildren<Collider3D>()[0]->SetDiscoverableByRayCast(true);
                    }
                }

                editMode = false;
            }
        }

        // Center text
        ImVec2 textSize = ImGui::CalcTextSize(asset.name.c_str());

        if (textSize.x < ImGui::GetContentRegionAvail().x)
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetContentRegionAvail().x - textSize.x) / 2);

        if (SelectedAsset == &asset)
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s", asset.name.c_str());
        else
            ImGui::Text("%s", asset.name.c_str());
    }
    ImGui::EndGroup();
}

bool EditorUI::IsMainRenderWindowIsHovered()
{
    return renderHovered[0];
}

void EditorUI::AddCamera(Camera3D *camera)
{
    cameras.push_back(camera);
    camera->SetAutoUpdateStateEnabled(false);
}

void EditorUI::HandleCameraStateUpdates(int num)
{
    EXPECT_ERROR(num >= cameras.size(), "Camera index is out of range");

    if (renderHovered[num])
    {
        cameras[num]->Update();
    }
}

void EditorUI::UpdateCamerasStates()
{
    for (int i = 0; i < cameras.size(); i++)
        HandleCameraStateUpdates(i);
}

void EditorUI::drawGameObjectsTree(GameObject *parent)
{
    for (GameObject *gameObject: *gameObjects)
    {
        if (gameObject->GetParent() == parent)
        {
            ImGuiTreeNodeFlags flags = 0;

            if (gameObject->GetChildren<GameObject3D>().size() == 0)
                flags |= ImGuiTreeNodeFlags_Leaf;

            if (gameObject == SelectedGameObject)
                flags |= ImGuiTreeNodeFlags_Selected;

            ImGui::PushID(gameObject);
            if (ImGui::TreeNodeEx(gameObject->GetName().c_str(), flags))
            {
                if (ImGui::IsItemHovered())
                {
                    ImGui::BeginTooltip();
                    ImGui::Text("%s", gameObject->GetName().c_str());
                    ImGui::EndTooltip();
                }

                if (ImGui::IsItemClicked())
                {
                    Model *model = dynamic_cast<Model *>(gameObject);

                    if (model)
                    {
                        if (SelectedGameObject)
                            SelectedGameObject->GetChildren<Collider3D>()[0]->SetColor(Color::Cyan());

                        if (model->GetChildren<Collider3D>().size())
                            model->GetChildren<Collider3D>()[0]->SetDiscoverableByRayCast(false);

                        SelectedGameObject = gameObject;
                        RememberTransforms();
                        SetEditMode(true);
                    }

                    MoveWithMouse = false;
                }

                drawGameObjectsTree(gameObject);
                ImGui::TreePop();
            }
            ImGui::PopID();
        }
    }
}

void EditorUI::UpdateSelectedObjectProperties()
{
    if (SelectedGameObject != nullptr)
    {
        GameObject3D *gameObject = dynamic_cast<GameObject3D *>(SelectedGameObject);
        Vector3 pos = gameObject->GetPosition();
        Euler rot = gameObject->GetRotation();

        selectedObjectName = SelectedGameObject->GetName();

        selectedObjectPosition[0] = pos.x;
        selectedObjectPosition[1] = pos.y;
        selectedObjectPosition[2] = pos.z;

        selectedObjectRotation[0] = rot.Pitch();
        selectedObjectRotation[1] = rot.Yaw();
        selectedObjectRotation[2] = rot.Roll();
    }
    else
    {
        selectedObjectName = "None";

        selectedObjectPosition[0] = 0;
        selectedObjectPosition[1] = 0;
        selectedObjectPosition[2] = 0;

        selectedObjectRotation[0] = 0;
        selectedObjectRotation[1] = 0;
        selectedObjectRotation[2] = 0;
    }
}

void EditorUI::applySelectedObjectsEdits()
{
    if (SelectedGameObject != nullptr)
    {
        GameObject3D *gameObject = dynamic_cast<GameObject3D *>(SelectedGameObject);
//        gameObject->SetPosition({selectedObjectPosition[0], selectedObjectPosition[1], selectedObjectPosition[2]});
        gameObject->SetRotation({selectedObjectRotation[0], selectedObjectRotation[1], selectedObjectRotation[2]});
        PrevObjectRotation = gameObject->GetRotation();
    }
}

void EditorUI::SetEditMode(bool editMode)
{
    EXPECT_ERROR(editMode && !SelectedGameObject, "Can't enter edit mode. No object is selected.");
    this->editMode = editMode;

    if (!editMode)
    {
        Model *model = dynamic_cast<Model *>(SelectedGameObject);

        if (model)
        {
            model->GetChildren<Collider3D>()[0]->SetColor(Color::Cyan());
            model->GetChildren<Collider3D>()[0]->SetDiscoverableByRayCast(true);

            if (MoveWithMouse)
            {
                model->SetPosition(editModeInitialPosition);
                model->SetRotation(editModeInitialRotation);
            }
        }

        MoveWithMouse = false;
        SelectedGameObject = nullptr;
    }

    if (editMode)
    {
        Model *model = dynamic_cast<Model *>(SelectedGameObject);
        EXPECT_ERROR(!model, "Can't enter edit mode, selected object is not a model");
        model->GetChildren<Collider3D>()[0]->SetDiscoverableByRayCast(false);
    }
}

void EditorUI::RestoreTransforms()
{
    if (SelectedGameObject && editMode)
    {
        Model *model = dynamic_cast<Model *>(SelectedGameObject);
        model->SetPosition(editModeInitialPosition);
        model->SetRotation(editModeInitialRotation);
    }
}

void EditorUI::readConfig()
{
    DrawCollisionShapes = Config::GetInstance().GetValue<bool>("EditorUI", "DrawCollisionShapes");
    prevDrawCollisionShapes = DrawCollisionShapes;

    ShowAxis = Config::GetInstance().GetValue<bool>("EditorUI", "ShowAxis");
    prevShowAxis = ShowAxis;
    Engine::GetInstance().SetDrawAxisLines(ShowAxis);

    ShowGrid = Config::GetInstance().GetValue<bool>("EditorUI", "ShowGrid");
    prevShowGrid = ShowGrid;
    Engine::GetInstance().SetDrawGrid(ShowGrid);
}

void EditorUI::RememberTransforms()
{
    Model *model = dynamic_cast<Model *>(SelectedGameObject);

    if (model)
    {
        editModeInitialPosition = model->GetPosition();
        editModeInitialRotation = model->GetRotation();
        PrevObjectRotation = model->GetRotation();
    }
}
