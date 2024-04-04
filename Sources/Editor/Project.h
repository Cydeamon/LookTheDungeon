#pragma once

#include <Engine.h>
#include <json.hpp>
#include <CMakeConfig.h>

class Project
{
public:
    Project(std::string projectPath = "");

    void Save(std::vector<GameObject3D *> objects);
    void OpenFromFile();

    void SetProjectPath(const std::string& path) { projectPath = path; }
    std::string GetFileName() { return fileName; }
    std::vector<GameObject3D *> GetGameObjects();

    void SetModified();
    bool IsModified();
private:
    std::string projectPath;
    std::string fileName;

    void getProjectPathFromSaveDialog();
    void getProjectPathFromOpenDialog();
    nlohmann::json getGameObjectJson(GameObject3D *gameObject);
    GameObject3D *readGameObjectFromJson(nlohmann::json jsonObject);
    bool modified = false;
};
