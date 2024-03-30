#pragma once

#include <Engine.h>
#include "Objects/Cameras/Camera3D.h"
#include "Input/Input.h"

class IsometricCamera : public Camera3D
{
public:
    IsometricCamera();
    void Update() override;
    std::string GetName() const override { return "Isometric camera"; }

    double Speed = 30.0f;
    float MouseSensitivity = 0.5;
};
