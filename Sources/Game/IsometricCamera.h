#pragma once

#include <Engine.h>
#include "Objects/Cameras/Camera3D.h"
#include "Input/Input.h"

class IsometricCamera : public Camera3D
{
public:
    IsometricCamera();
    void Update() override;

    double Speed = 30.0f;
    float MouseSensitivity = 0.5;
};
