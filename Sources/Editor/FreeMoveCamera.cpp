#include "FreeMoveCamera.h"


FreeMoveCamera::FreeMoveCamera() : Camera3D()
{
    SetPosition(-1.42671, 1.81062, 2.85435);
    SetRotation(Euler(-12, -62, 0));
    rotation.Clamp(Vector3(-89, 0, 0), Vector3(89, 0, 0));
}

void FreeMoveCamera::Update()
{
    Camera3D::Update();
    Vector3 right = GetRotation().ResetPitch().Direction().CrossProduct(Vector3::Up());

    // Handle keyboard input
    if (Input::IsPressed(W))
        SetPosition(GetPosition() + GetRotation().Direction() * Speed * Engine::GetInstance().GetDelta());
    if (Input::IsPressed(S))
        SetPosition(GetPosition() - GetRotation().Direction() * Speed * Engine::GetInstance().GetDelta());
    if (Input::IsPressed(A))
        SetPosition(GetPosition() - right * Speed * Engine::GetInstance().GetDelta());
    if (Input::IsPressed(D))
        SetPosition(GetPosition() + right * Speed * Engine::GetInstance().GetDelta());

    // Handle mouse input
    if (Input::IsJustPressed(MouseButton::RIGHT))
    {
        Input::ToggleMouseCapture();
    }

    if (Input::IsMouseCaptured() && Input::GetMouseMovementDelta())
    {
        MouseRotation(Input::GetMouseMovementDelta().X * MouseSensitivity, Input::GetMouseMovementDelta().Y * MouseSensitivity);
    }
}
