#include "EditorUI.h"
#include <windows.h>

EditorUI::EditorUI()
{
    ImGui::LoadIniSettingsFromDisk("EditorUI.ini");
    ImGui::GetIO().IniFilename = "EditorUI.ini";
    ImGui::StyleColorsDark();
    Engine::GetInstance().AddFrameBuffers(1);
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

            }
            ImGui::End();

            ImGui::Begin("Level Assets");
            {

            }
            ImGui::End();

            ImGui::Begin("Editor Settings");
            {

            }
            ImGui::End();

            ImGui::Begin("Selected Object Transforms");
            {

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
        }
        ImGui::End();
    }
}

void EditorUI::NotImplementedWarning()
{
    EXPECT_ERROR(1, "This functionality is not implemented");
}
