#pragma once

#include "../Engine/Engine.h"
#include "FreeMoveCamera.h"
#include "IsometricCamera.h"
#include "EditorUI.h"

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
    RayCast3D *isometricCameraMouseRayCast = nullptr;
    std::vector <Model*> levelObjects;
    ColliderCube *hoveredCollider = nullptr;
    Euler rotationModeIntermediate;
    int floor = 0;

    void init();
    void update();
    void handleInput();
    void setupUI();
    void handleSelectedAssetPlacement();

    EditorUI *editorUI = nullptr;
};