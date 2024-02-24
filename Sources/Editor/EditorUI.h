#pragma once

#include "Engine.h"

class EditorUI
{
public:
    static EditorUI& GetInstance();
    void Update();

private:
    EditorUI();
    ~EditorUI();
    void SetupLayout();
    void NotImplementedWarning();
};
