#include "PauseController.h"
#include "Input/Input.h"

bool PauseController::IsPauseTriggered() const {
    return KetaEngine::Input::GetInstance()->TriggerKey(KeyboardKey::P) ||
           KetaEngine::Input::IsTriggerPad(0, GamepadButton::Start);
}
