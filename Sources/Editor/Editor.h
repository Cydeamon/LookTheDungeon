#pragma once

#include "../Engine/Engine.h"
#include "FreeMoveCamera.h"
#include "IsometricCamera.h"
#include "EditorUI.h"
#include "Project.h"

class Editor
{
public:
    Editor();
    ~Editor();

    void Run();

private:
    Engine* engine;
    IsometricCamera *isometricCamera = nullptr;
    FreeMoveCamera *freeMoveCamera = nullptr;
    RayCast3D *editorMouseRaycast = nullptr;
    std::vector <GameObject3D *> levelObjects;
    Collider3D *hoveredCollider = nullptr;
    Euler rotationModeIntermediate;
    int floor = 0;
    EditorUI *editorUI = nullptr;
    Project *project = nullptr;

    void init();
    void update();
    void handleInput();
    void handleSelectedAssetPlacement();

    void quit();
    void newProject();
    void saveProject();
    void openProject();

    void clearLevelObjects();
    void updateTitle();
    bool askUserForSave();
    ShaderProgram *mainShaderProgram;
};