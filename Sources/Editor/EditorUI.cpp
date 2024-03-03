#include "EditorUI.h"
#include <windows.h>

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
                prevShowGrid = ShowGrid;
            }

            if (ShowAxis != prevShowAxis)
            {
                Engine::GetInstance().SetDrawAxisLines(ShowAxis);
                prevShowAxis = ShowAxis;
            }
        }
        ImGui::End();

        ImGui::Begin("Selected Object Transforms");
        {
            ImGui::Text("Selected Object: %s", selectedObjectName.c_str());
            ImGui::InputFloat3("Position", &selectedObjectPosition[0]);
            ImGui::InputFloat3("Rotation", &selectedObjectRotation[0]);
        }
        ImGui::End();

        ImGui::Begin("Statistics");
        {
            ImGui::Text("FPS: %i", Engine::GetInstance().GetFPS());
            ImGui::Text("Average FPS: %i", Engine::GetInstance().GetAverageFPS());
            ImGui::Text("Elements: %i", Engine::GetInstance().GetGameObjectsCount());
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

void EditorUI::fillAssets(const std::string& path, std::vector<Asset> *assets)
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
                {pos.x + ImGui::GetContentRegionAvail().x + 8, pos.y + 120},
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

    if (renderHovered[num]) {
        cameras[num]->Update();
    }
}

void EditorUI::UpdateCamerasStates()
{
    for (int i = 0; i < cameras.size(); i++)
        HandleCameraStateUpdates(i);
}
