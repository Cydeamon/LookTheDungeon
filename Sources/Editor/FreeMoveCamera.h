#pragma once

#include <Engine.h>
#include "Objects/Cameras/Camera3D.h"
#include "Input/Input.h"

class FreeMoveCamera : public Camera3D
{
public:
    FreeMoveCamera();
    void Update() override;
    std::string GetName() const override { return "Free move camera"; }

    double Speed = 30.0f;
    float MouseSensitivity = 0.5;
};
