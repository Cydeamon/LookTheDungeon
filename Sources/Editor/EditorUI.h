#pragma once

#include "Engine.h"

class EditorUI
{
public:
    static EditorUI& GetInstance();
    void Update();
    bool IsMainRenderWindowIsHovered();

    void AddCamera(Camera3D *camera);
    void HandleCameraStateUpdates(int num);
    void UpdateCamerasStates();

    void SetStickToGrid(bool stickToGrid);
    void SetShowGrid(bool showGrid);
    void SetShowAxis(bool showAxis);
    void SetGridSize(float gridSize);

    float GetGridSize();
    bool GetStickToGrid();
    bool GetShowGrid();
    bool GetShowAxis();

    struct Asset
    {
        std::string name;
        std::string path;
        std::string texturePath;
        Texture* texture = nullptr;
    };

    Asset *SelectedAsset = nullptr;
    float FloorHeight = 0;
    bool StickToGrid = true;
    bool ShowGrid = true;
    bool ShowAxis = true;
    float StickyGridSize[2] = {1, 1};

private:

    EditorUI();
    ~EditorUI();

    std::vector<Asset> levelParts;
    std::vector<Asset> characters;
    Texture *assetPlaceholderTexture = nullptr;
    bool prevStickToGrid = false;
    bool prevShowGrid = false;
    bool prevShowAxis = false;
    std::string selectedObjectName = "Selected object name";
    float selectedObjectPosition[3] = {0, 0, 0};
    float selectedObjectRotation[3] = {0, 0, 0};
    std::vector<Camera3D *> cameras;
    std::vector<bool> renderHovered;

    void SetupLayout();
    static void NotImplementedWarning();
    void DrawAsset(Asset &asset);
    void fillAssets(const std::string& path, std::vector<Asset> *assets);
};
