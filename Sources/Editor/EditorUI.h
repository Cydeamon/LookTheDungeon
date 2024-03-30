#pragma once

#include "Engine.h"

class EditorUI
{
public:
    static EditorUI& GetInstance();
    void Update();
    bool IsRenderWindowHovered(int num);

    void AddCamera(Camera3D *camera);
    void HandleCameraStateUpdates(int num);
    void UpdateCamerasStates();
    void UpdateSelectedObjectProperties();

    void SetEditMode(bool editMode);
    bool IsEditMode() { return editMode; }

    void SetSaveProjectCallback(std::function<void()> callback) { saveProjectCallback = callback; }
    void SetOpenProjectCallback(std::function<void()> callback) { openProjectCallback = callback; }
    void SetNewProjectCallback(std::function<void()> callback) { newProjectCallback = callback; }
    void SetExitCallback(std::function<void()> callback) { exitCallback = callback; }

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
    bool MoveWithMouse = false;
    float StickyGridSize[2] = {1, 1};
    float StickyGridOffset[2] = {0, 0};
    Euler PrevObjectRotation = Euler(0, -90, 0);
    bool RotationMode = false;

    void RestoreTransforms();
    void RememberTransforms();
private:

    EditorUI();
    ~EditorUI();

    void SetupLayout();
    static void NotImplementedWarning();
    void DrawAsset(Asset &asset);
    void fillAssets(const std::string& path, std::vector<Asset> *assets);
    void drawGameObjectsTree(GameObject *parent = nullptr);
    void applySelectedObjectsEdits();

    std::function<void()> newProjectCallback;
    std::function<void()> saveProjectCallback;
    std::function<void()> openProjectCallback;
    std::function<void()> exitCallback;

    std::vector<Asset> levelParts;
    std::vector<Asset> characters;
    Texture *assetPlaceholderTexture = nullptr;
    bool prevStickToGrid = false;
    bool prevShowGrid = false;
    bool prevShowAxis = false;
    bool prevDrawCollisionShapes = false;
    std::string selectedObjectName = "None";
    float selectedObjectPosition[3] = {0, 0, 0};
    float selectedObjectRotation[3] = {0, 0, 0};
    Vector3 editModeInitialPosition;
    Euler editModeInitialRotation;
    std::vector<Camera3D *> cameras;
    std::vector<bool> renderHovered;
    std::vector<GameObject*> *gameObjects;
    bool editMode = false;
    void readConfig();
};
