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
    void UpdateSelectedObjectProperties();

    void SetStickToGrid(bool stickToGrid);
    void SetShowGrid(bool showGrid);
    void SetShowAxis(bool showAxis);
    void SetGridSize(float gridSize);
    void SetEditMode(bool editMode);

    float GetGridSize();
    bool GetStickToGrid();
    bool GetShowGrid();
    bool GetShowAxis();
    bool IsEditMode() { return editMode; }

    struct Asset
    {
        std::string name;
        std::string path;
        std::string texturePath;
        Texture* texture = nullptr;
    };

    Asset *SelectedAsset = nullptr;
    GameObject *SelectedGameObject = nullptr;
    float FloorHeight = 0;
    bool StickToGrid = true;
    bool ShowGrid = true;
    bool ShowAxis = true;
    bool DrawCollisionShapes = true;
    float StickyGridSize[2] = {1, 1};
    float StickyGridOffset[2] = {0.5, 0.5};

private:

    EditorUI();
    ~EditorUI();

    void SetupLayout();
    static void NotImplementedWarning();
    void DrawAsset(Asset &asset);
    void fillAssets(const std::string& path, std::vector<Asset> *assets);
    void drawGameObjectsTree(GameObject *parent = nullptr);
    void applySelectedObjectsEdits();

    std::vector<Asset> levelParts;
    std::vector<Asset> characters;
    Texture *assetPlaceholderTexture = nullptr;
    bool prevStickToGrid = false;
    bool prevShowGrid = false;
    bool prevShowAxis = false;
    std::string selectedObjectName = "None";
    float selectedObjectPosition[3] = {0, 0, 0};
    float selectedObjectRotation[3] = {0, 0, 0};
    Vector3 editModeInitialPosition;
    Euler editModeInitialRotation;
    std::vector<Camera3D *> cameras;
    std::vector<bool> renderHovered;
    std::vector<GameObject*> *gameObjects;
    bool editMode = false;
};
