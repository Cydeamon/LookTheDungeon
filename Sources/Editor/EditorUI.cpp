#include "EditorUI.h"
#include <windows.h>

EditorUI::EditorUI()
{
    ImGui::LoadIniSettingsFromDisk("EditorUI.ini");
    ImGui::GetIO().IniFilename = "EditorUI.ini";
    ImGui::StyleColorsDark();
    Engine::GetInstance().AddFrameBuffers(1);

    assetPlaceholderTexture = new Texture("Assets/tempModelPreview.jpg");
    fillAssets("Assets/Models/LevelParts", &levelParts);
    fillAssets("Assets/Models/Characters", &characters);
}

EditorUI::~EditorUI()
{
}

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
            int assetsTablesColumnsCount = ImGui::GetContentRegionAvail().x / 110;
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
            int assetsTablesColumnsCount = ImGui::GetContentRegionAvail().x / 110;
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
            ImGui::Checkbox("Draw grid", &showGrid);
            ImGui::SameLine();
            ImGui::Checkbox("Draw axis", &showAxis);
            ImGui::SameLine();
            ImGui::Checkbox("Stick to grid", &stickToGrid);

            ImGui::Text("Grid size");
            ImGui::SameLine();
            ImGui::InputFloat("##gridSize", &gridSize);
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
        ImGui::Begin("Render");
        {
            Engine::GetInstance().DrawFramebufferToImGuiWindow(0);

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

void EditorUI::fillAssets(std::string path, std::vector<Asset> *assets)
{
    std::string category = path.substr(path.find_last_of("/") + 1);

    for (const auto &entry: std::filesystem::directory_iterator(path))
    {
        if (entry.path().extension() == ".glb")
        {
            Asset asset;
            asset.path = entry.path().string();
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
            NotImplementedWarning();
        }

        ImGui::Text("%s", asset.name.c_str());
    }
    ImGui::EndGroup();
}
