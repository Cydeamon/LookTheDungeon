#pragma once

#include "Engine.h"

class EditorUI
{
public:
    static EditorUI& GetInstance();
    void Update();
    bool IsMainRenderWindowIsHovered();

    void AddCamera(Camera3D *camera);
    void HandleInputForCamera(int num);
    void HandleCamerasInputs();
private:
    struct Asset
    {
        std::string name;
        std::string path;
        std::string texturePath;
        Texture* texture = nullptr;
    };

    EditorUI();
    ~EditorUI();

    std::vector<Asset> levelParts;
    std::vector<Asset> characters;
    Texture *assetPlaceholderTexture = nullptr;
    bool stickToGrid = true;
    bool prevStickToGrid = false;
    bool showGrid = true;
    bool prevShowGrid = false;
    bool showAxis = true;
    bool prevShowAxis = false;
    float gridSize = 1;
    std::string selectedObjectName = "Selected object name";
    float selectedObjectPosition[3] = {0, 0, 0};
    float selectedObjectRotation[3] = {0, 0, 0};
    Asset *selectedAsset = nullptr;
    std::vector<Camera3D *> cameras;
    std::vector<bool> renderHovered;

    void SetupLayout();
    static void NotImplementedWarning();
    void DrawAsset(Asset &asset);
    void fillAssets(const std::string& path, std::vector<Asset> *assets);
};
