#pragma once

#include <Engine.h>
#include <json.hpp>
#include <CMakeConfig.h>

class Project
{
public:
    Project(std::string projectPath = "");

    void Save(std::vector<GameObject3D *> objects);
    void SetProjectPath(const std::string& path) { projectPath = path; }

    std::vector<GameObject3D *> GetGameObjects();
private:
    std::string projectPath;

    void getProjectPathFromSaveDialog();
    void loadProject(std::string projectPath);
    nlohmann::json getGameObjectJson(GameObject3D *gameObject);
    GameObject3D *readGameObjectFromJson(nlohmann::json jsonObject);
};
