#include "IsometricCamera.h"

IsometricCamera::IsometricCamera()
{
    SetOrthographicMode(true);
    SetFOV(6);
    SetPosition(-35, 50, -35);
    SetRotation(Euler(-45, 45, 0));
}

void IsometricCamera::Update()
{
    Camera3D::Update();

    Vector3 right = GetRotation().ResetPitch().Direction().CrossProduct(Vector3::Up());
    Vector3 forward = -right.CrossProduct(Vector3::Up());

    // Handle keyboard input
    if (Input::IsPressed(W))
        SetPosition(GetPosition() + forward * Speed * Engine::GetInstance().GetDelta());
    if (Input::IsPressed(S))
        SetPosition(GetPosition() - forward * Speed * Engine::GetInstance().GetDelta());
    if (Input::IsPressed(A))
        SetPosition(GetPosition() - right * Speed * Engine::GetInstance().GetDelta());
    if (Input::IsPressed(D))
        SetPosition(GetPosition() + right * Speed * Engine::GetInstance().GetDelta());

    // Handle mouse scroll to zoom
    if (!Input::IsPressed(MouseButton::RIGHT))
    {
        if (Input::IsJustPressed(MouseButton::SCROLL_DOWN))
        {
            SetFOV(GetFOV() + 1);

            if (GetFOV() > 15)
                SetFOV(15);
        }

        if (Input::IsJustPressed(MouseButton::SCROLL_UP))
        {
            SetFOV(GetFOV() - 2);

            if (GetFOV() < 2)
                SetFOV(2);
        }
    }
}

