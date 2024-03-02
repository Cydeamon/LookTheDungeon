#pragma once

#include "../Engine/Engine.h"
#include "FreeMoveCamera.h"
#include "IsometricCamera.h"

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

    void init();
    void update();
    void handleInput();
    void setupUI();
    void handleSelectedAssetPlacement();

};