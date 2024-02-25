#pragma once

#include "Engine.h"

class EditorUI
{
public:
    static EditorUI& GetInstance();
    void Update();

private:
    struct Asset
    {
        std::string name;
        std::string path;
        Texture* texture = nullptr;
    };

    std::vector<Asset> levelParts;
    std::vector<Asset> characters;

    EditorUI();
    ~EditorUI();
    void SetupLayout();
    void NotImplementedWarning();
    void DrawAsset(Asset &asset);
    void fillAssets(std::string path, std::vector<Asset> *assets);
    Texture *assetPlaceholderTexture = nullptr;
};
