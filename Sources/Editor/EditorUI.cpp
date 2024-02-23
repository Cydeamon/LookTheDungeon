#include "EditorUI.h"

EditorUI &EditorUI::GetInstance()
{
    static EditorUI instance;
    return instance;
}

void EditorUI::Update()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::ShowDemoWindow();
}

EditorUI::EditorUI()
{
    ImGui::StyleColorsDark();
}

EditorUI::~EditorUI()
{

}
