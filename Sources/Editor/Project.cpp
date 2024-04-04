#include "Project.h"
#include <filesystem>

#if IS_WINDOWS

    #include <windows.h>
    #include <commdlg.h>

#elif IS_LINUX
    #include <gtk/gtk.h>
#endif

Project::Project(std::string projectPath)
{
    this->projectPath = projectPath;

    if (projectPath != "")
        fileName = projectPath.substr(projectPath.find_last_of('/') + 1);
}

void Project::Save(std::vector<GameObject3D *> gameObjects)
{
    if (projectPath == "")
        getProjectPathFromSaveDialog();

    if (projectPath == "")
    {
        throw std::runtime_error("Project save canceled");
    }
    else
    {
        nlohmann::json levelDataJson = {
            {"AppVersion", PROJECT_VER},
            {"GameObjects", nlohmann::json::array()},
        };

        for (GameObject3D *gameObject: gameObjects)
        {
            if (gameObject->GetParent() == nullptr)
            {
                levelDataJson["GameObjects"].push_back(getGameObjectJson(gameObject));
            }
        }

        std::ofstream file(projectPath);
        file << levelDataJson.dump(4) << std::endl;
        file.close();
    }

    modified = false;
}

void Project::OpenFromFile()
{
    getProjectPathFromOpenDialog();

    if (projectPath == "")
        throw std::runtime_error("Project open canceled");    
}

void Project::getProjectPathFromSaveDialog()
{
    std::filesystem::path cwd = std::filesystem::current_path();
    char buffer[512] = "\0";
    strncpy(buffer, (cwd.string() + "\\level.lvl").c_str(), 512);

    #if IS_WINDOWS
    {
        OPENFILENAME ofn;
        ZeroMemory(&ofn, sizeof(OPENFILENAME));
        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.hwndOwner = NULL;
        ofn.lpstrFile = buffer;
        ofn.nMaxFile = sizeof(buffer);
        ofn.lpstrFilter = "Level Files (*.lvl)\0*.lvl\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = NULL;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        if (GetSaveFileName(&ofn))
            projectPath = buffer;

        if (projectPath != "")
            fileName = projectPath.substr(projectPath.find_last_of('/') + 1);

        std::filesystem::current_path(cwd);
    }
    #elif IS_LINUX
    {

        gtk_init(NULL, NULL);
        GtkWidget *dialog = gtk_file_chooser_dialog_new(
            "Save level",
            NULL,
            GTK_FILE_CHOOSER_ACTION_SAVE,
            GTK_STOCK_CANCEL, 
            GTK_RESPONSE_CANCEL,
            GTK_STOCK_SAVE, 
            GTK_RESPONSE_ACCEPT,
            NULL
        );

        gint response = gtk_dialog_run(GTK_DIALOG(dialog));

        if (response == GTK_RESPONSE_ACCEPT)
            projectPath = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

        if (projectPath != "")
            fileName = projectPath.substr(projectPath.find_last_of('/') + 1);

        gtk_widget_destroy(dialog);
        gtk_main_quit();
        std::filesystem::current_path(cwd);
    }
    #endif
}

void Project::getProjectPathFromOpenDialog()
{
    std::filesystem::path cwd = std::filesystem::current_path();
    char buffer[512] = "\0";

    #if IS_WINDOWS
    {
        OPENFILENAME ofn;
        ZeroMemory(&ofn, sizeof(OPENFILENAME));
        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.hwndOwner = NULL;
        ofn.lpstrFile = buffer;
        ofn.nMaxFile = sizeof(buffer);
        ofn.lpstrFilter = "Level Files (*.lvl)\0*.lvl\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = NULL;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        if (GetOpenFileName(&ofn))
        {
            std::replace(buffer, buffer + strlen(buffer), '\\', '/');
            projectPath = buffer;
        }

        if (projectPath != "")
            fileName = projectPath.substr(projectPath.find_last_of('/') + 1);

        std::filesystem::current_path(cwd);
    }
    #elif IS_LINUX
    {
        gtk_init(NULL, NULL);
        GtkWidget *dialog = gtk_file_chooser_dialog_new(
            "Open level",
            NULL,
            GTK_FILE_CHOOSER_ACTION_OPEN,
            GTK_STOCK_CANCEL, 
            GTK_RESPONSE_CANCEL,
            GTK_STOCK_OPEN, 
            GTK_RESPONSE_ACCEPT,
            NULL
        );

        gint response = gtk_dialog_run(GTK_DIALOG(dialog));

        if (response == GTK_RESPONSE_ACCEPT)
            projectPath = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

        if (projectPath != "")
            fileName = projectPath.substr(projectPath.find_last_of('/') + 1);

        gtk_widget_destroy(dialog);
        gtk_main_quit();
        std::filesystem::current_path(cwd);
    }
    #endif
}

nlohmann::json Project::getGameObjectJson(GameObject3D *gameObject)
{
    EXPECT_ERROR(gameObject == nullptr, "Can't save null game object");
    nlohmann::json gameObjectJson;

    gameObjectJson["Name"] = gameObject->GetName();
    gameObjectJson["Children"] = nlohmann::json::array();

    for (GameObject *child: *gameObject->GetChildren())
        gameObjectJson["Children"].push_back(getGameObjectJson(dynamic_cast<GameObject3D *>(child)));

    if (Collider3D *collider = dynamic_cast<Collider3D *>(gameObject))
    {
        gameObjectJson["Type"] = "Collider";
        gameObjectJson["Meshes"] = nlohmann::json::array();
        const std::vector<Mesh> &meshes = collider->GetMeshes();

        for (Mesh mesh: meshes)
        {
            std::vector<Vertex> vertices = mesh.GetVertices();
            std::vector<unsigned int> indices = mesh.GetIndices();

            nlohmann::json meshJson = {
                {"Vertices",        nlohmann::json::array()},
                {"VerticesNormals", nlohmann::json::array()},
                {"Indices",         nlohmann::json::array()},
                {"Name",            mesh.GetName()}
            };

            for (Vertex vertex: vertices)
            {
                meshJson["Vertices"].push_back(
                    {
                        vertex.Position.x, vertex.Position.y, vertex.Position.z
                    }
                );

                meshJson["VerticesNormals"].push_back(
                    {
                        vertex.Normal.x, vertex.Normal.y, vertex.Normal.z
                    }
                );
            }

            for (unsigned int index: indices)
                meshJson["Indices"].push_back(index);

            gameObjectJson["Meshes"].push_back(meshJson);
        }
    }
    else if (Model *model = dynamic_cast<Model *>(gameObject))
    {
        gameObjectJson["Type"] = "Model";
        gameObjectJson["Path"] = model->GetModelPath();

        if (model->GetCurrentAnimation())
            gameObjectJson["Animation"] = model->GetCurrentAnimation()->GetName();
    }
    else
    {
        EXPECT_ERROR(1, "Can't save level! Unknown game object type. Object: %s", gameObject->GetName().c_str());
    }

    gameObjectJson["Position"] = {gameObject->GetPosition().x, gameObject->GetPosition().y, gameObject->GetPosition().z};
    gameObjectJson["Rotation"] = {gameObject->GetRotation().Pitch(), gameObject->GetRotation().Yaw(), gameObject->GetRotation().Roll()};
    gameObjectJson["Scale"] = {gameObject->GetScale().x, gameObject->GetScale().y, gameObject->GetScale().z};

    return gameObjectJson;
}

std::vector<GameObject3D *> Project::GetGameObjects()
{
    std::vector<GameObject3D *> result;
    nlohmann::json levelDataJson = nlohmann::json::parse(std::ifstream(projectPath));

    for (nlohmann::json gameObjectJson: levelDataJson["GameObjects"])
    {
        result.push_back(readGameObjectFromJson(gameObjectJson));
    }

    return result;
}

GameObject3D *Project::readGameObjectFromJson(nlohmann::json jsonObject)
{
    GameObject3D *gameObject = nullptr;

    if (jsonObject["Type"] == "Collider")
    {
        std::vector<Mesh> meshes;

        for (nlohmann::json meshJson: jsonObject["Meshes"])
        {
            std::vector<Vertex> vertices;
            std::vector<unsigned int> indices;
            std::vector<Vector3> verticesNormals;

            for (int i = 0; i < meshJson["Vertices"].size(); i++)
            {
                Vertex vertex {};
                vertex.Position = glm::vec3(meshJson["Vertices"][i][0], meshJson["Vertices"][i][1], meshJson["Vertices"][i][2]);
                vertex.Normal = glm::vec3(meshJson["VerticesNormals"][i][0], meshJson["VerticesNormals"][i][1], meshJson["VerticesNormals"][i][2]);
                vertices.push_back(vertex);
            }

            for (int i = 0; i < meshJson["Indices"].size(); i++)
                indices.push_back(meshJson["Indices"][i]);

            meshes.push_back(Mesh(vertices, indices, {}, jsonObject["Name"].get<std::string>()));
        }

        gameObject = new Collider3D(meshes);
    }
    else if (jsonObject["Type"] == "Model")
    {
        std::string cwd = std::filesystem::current_path().string();
        Model *model = new Model(jsonObject["Path"].get<std::string>());
        gameObject = model;

        if (jsonObject["Animation"].size() > 0)
            model->PlayAnimation(jsonObject["Animation"].get<std::string>(), LOOP);
    }
    else
    {
        EXPECT_ERROR(1, "Can't load level! Unknown game object type. Object: %s", jsonObject.value("Name", "").c_str());
    }

    gameObject->SetName(jsonObject["Name"].get<std::string>());
    gameObject->SetPosition(jsonObject["Position"][0], jsonObject["Position"][1], jsonObject["Position"][2]);
    gameObject->SetRotation(Euler(jsonObject["Rotation"][0], jsonObject["Rotation"][1], jsonObject["Rotation"][2]));
    gameObject->SetScale(jsonObject["Scale"][0], jsonObject["Scale"][1], jsonObject["Scale"][2]);

    if (jsonObject["Children"].size() != 0)
    {
        for (nlohmann::json childJson: jsonObject["Children"])
            gameObject->AddChild(readGameObjectFromJson(childJson));
    }

    EXPECT_ERROR(gameObject == nullptr, "Can't load level! Game object processing failure. Object: %s", jsonObject.value("Name", "").c_str());
    return gameObject;
}

void Project::SetModified()
{
    modified = true;
}

bool Project::IsModified()
{
    return modified;
}
