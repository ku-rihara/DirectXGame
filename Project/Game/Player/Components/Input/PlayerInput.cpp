#include "PlayerInput.h"
#include "input/Input.h"

Vector3 PlayerInput::GetMoveDirection() const {
    Vector3 velocity         = {0.0f, 0.0f, 0.0f};
    KetaEngine::Input* input = KetaEngine::Input::GetInstance();

    if (input->PushKey(KeyboardKey::W)) { velocity.z += 1.0f; }
    if (input->PushKey(KeyboardKey::S)) { velocity.z -= 1.0f; }
    if (input->PushKey(KeyboardKey::A)) { velocity.x -= 1.0f; }
    if (input->PushKey(KeyboardKey::D)) { velocity.x += 1.0f; }

    Vector2 stickInput = KetaEngine::Input::GetPadStick(0, 0);
    velocity.x += stickInput.x;
    velocity.z += stickInput.y;

    return velocity;
}

bool PlayerInput::IsMoving() const {
    return GetMoveDirection().Length() > kMoveThreshold;
}

bool PlayerInput::IsDashInput() const {
    return KetaEngine::Input::IsPressPad(0, GamepadButton::LB);
}

bool PlayerInput::IsJumpKeyPressed() const {
    return KetaEngine::Input::GetInstance()->PushKey(KeyboardKey::J);
}

bool PlayerInput::IsJumpPadTriggered() const {
    return KetaEngine::Input::IsTriggerPad(0, GamepadButton::A);
}
