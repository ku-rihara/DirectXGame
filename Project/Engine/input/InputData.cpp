#include"InputData.h"


#include <imgui.h>

void ImGuiGamepadButtonSelector(const char* label, int32_t& target) {
    int currentIndex = GetGamepadButtonIndex(target);

    if (ImGui::Combo(label, &currentIndex, GamepadButtonLabels.data(), static_cast<int>(GamepadButtonLabels.size()))) {
        // インデックスから実際のボタン値に変換
        switch (currentIndex) {
        case 0:
            target = XINPUT_GAMEPAD_A;
            break;
        case 1:
            target = XINPUT_GAMEPAD_B;
            break;
        case 2:
            target = XINPUT_GAMEPAD_X;
            break;
        case 3:
            target = XINPUT_GAMEPAD_Y;
            break;
        case 4:
            target = XINPUT_GAMEPAD_LEFT_SHOULDER;
            break;
        case 5:
            target = XINPUT_GAMEPAD_RIGHT_SHOULDER;
            break;
        case 6:
            target = XINPUT_GAMEPAD_BACK;
            break;
        case 7:
            target = XINPUT_GAMEPAD_START;
            break;
        case 8:
            target = XINPUT_GAMEPAD_LEFT_THUMB;
            break;
        case 9:
            target = XINPUT_GAMEPAD_RIGHT_THUMB;
            break;
        case 10:
            target = XINPUT_GAMEPAD_DPAD_UP;
            break;
        case 11:
            target = XINPUT_GAMEPAD_DPAD_DOWN;
            break;
        case 12:
            target = XINPUT_GAMEPAD_DPAD_LEFT;
            break;
        case 13:
            target = XINPUT_GAMEPAD_DPAD_RIGHT;
            break;
        default:
            target = XINPUT_GAMEPAD_A;
            break;
        }
    }
}

void ImGuiKeyboardKeySelector(const char* label, int32_t& target) {
    int currentIndex = GetKeyboardKeyIndex(target);

    if (ImGui::Combo(label, &currentIndex, KeyboardKeyLabels.data(), static_cast<int>(KeyboardKeyLabels.size()))) {
        // インデックスから実際のキー値に変換
        // 文字キー A-Z
        if (currentIndex >= 0 && currentIndex <= 25) {
            target = DIK_A + currentIndex;
        }
        // 数字キー 0-9
        else if (currentIndex >= 26 && currentIndex <= 35) {
            target = DIK_0 + (currentIndex - 26);
        }
        // 特殊キー
        else {
            switch (currentIndex) {
            case 36:
                target = DIK_SPACE;
                break;
            case 37:
                target = DIK_RETURN;
                break;
            case 38:
                target = DIK_ESCAPE;
                break;
            case 39:
                target = DIK_BACK;
                break;
            case 40:
                target = DIK_TAB;
                break;
            case 41:
                target = DIK_LSHIFT;
                break;
            case 42:
                target = DIK_RSHIFT;
                break;
            case 43:
                target = DIK_LCONTROL;
                break;
            case 44:
                target = DIK_RCONTROL;
                break;
            case 45:
                target = DIK_LALT;
                break;
            case 46:
                target = DIK_RALT;
                break;
            case 47:
                target = DIK_UP;
                break;
            case 48:
                target = DIK_DOWN;
                break;
            case 49:
                target = DIK_LEFT;
                break;
            case 50:
                target = DIK_RIGHT;
                break;
            default:
                target = DIK_H;
                break;
            }
        }
    }
}

int32_t GetGamepadButtonIndex(const int32_t& buttonValue) {
    switch (buttonValue) {
    case XINPUT_GAMEPAD_A:
        return 0;
    case XINPUT_GAMEPAD_B:
        return 1;
    case XINPUT_GAMEPAD_X:
        return 2;
    case XINPUT_GAMEPAD_Y:
        return 3;
    case XINPUT_GAMEPAD_LEFT_SHOULDER:
        return 4;
    case XINPUT_GAMEPAD_RIGHT_SHOULDER:
        return 5;
    case XINPUT_GAMEPAD_BACK:
        return 6;
    case XINPUT_GAMEPAD_START:
        return 7;
    case XINPUT_GAMEPAD_LEFT_THUMB:
        return 8;
    case XINPUT_GAMEPAD_RIGHT_THUMB:
        return 9;
    case XINPUT_GAMEPAD_DPAD_UP:
        return 10;
    case XINPUT_GAMEPAD_DPAD_DOWN:
        return 11;
    case XINPUT_GAMEPAD_DPAD_LEFT:
        return 12;
    case XINPUT_GAMEPAD_DPAD_RIGHT:
        return 13;
    default:
        return 0;
    }
}


int32_t GetKeyboardKeyIndex(const int32_t& keyValue) {
    // 文字キー A-Z
    if (keyValue >= DIK_A && keyValue <= DIK_Z) {
        return keyValue - DIK_A;
    }
    // 数字キー 0-9
    if (keyValue >= DIK_0 && keyValue <= DIK_9) {
        return 26 + (keyValue - DIK_0);
    }

    // 特殊キー
    switch (keyValue) {
    case DIK_SPACE:
        return 36;
    case DIK_RETURN:
        return 37;
    case DIK_ESCAPE:
        return 38;
    case DIK_BACK:
        return 39;
    case DIK_TAB:
        return 40;
    case DIK_LSHIFT:
        return 41;
    case DIK_RSHIFT:
        return 42;
    case DIK_LCONTROL:
        return 43;
    case DIK_RCONTROL:
        return 44;
    case DIK_LALT:
        return 45;
    case DIK_RALT:
        return 46;
    case DIK_UP:
        return 47;
    case DIK_DOWN:
        return 48;
    case DIK_LEFT:
        return 49;
    case DIK_RIGHT:
        return 50;
    default:
        return 0;
    }
}


int32_t ToXInputButtonFlag(GamepadButton button) {
    switch (button) {
    case GamepadButton::A:
        return XINPUT_GAMEPAD_A;
    case GamepadButton::B:
        return XINPUT_GAMEPAD_B;
    case GamepadButton::X:
        return XINPUT_GAMEPAD_X;
    case GamepadButton::Y:
        return XINPUT_GAMEPAD_Y;
    case GamepadButton::LB:
        return XINPUT_GAMEPAD_LEFT_SHOULDER;
    case GamepadButton::RB:
        return XINPUT_GAMEPAD_RIGHT_SHOULDER;
    case GamepadButton::Back:
        return XINPUT_GAMEPAD_BACK;
    case GamepadButton::Start:
        return XINPUT_GAMEPAD_START;
    case GamepadButton::LS:
        return XINPUT_GAMEPAD_LEFT_THUMB;
    case GamepadButton::RS:
        return XINPUT_GAMEPAD_RIGHT_THUMB;
    case GamepadButton::DPadUp:
        return XINPUT_GAMEPAD_DPAD_UP;
    case GamepadButton::DPadDown:
        return XINPUT_GAMEPAD_DPAD_DOWN;
    case GamepadButton::DPadLeft:
        return XINPUT_GAMEPAD_DPAD_LEFT;
    case GamepadButton::DPadRight:
        return XINPUT_GAMEPAD_DPAD_RIGHT;
    default:
        return 0;
    }
}

GamepadButton FromXInputButtonFlag(const int32_t& xInputFlag) {
    switch (xInputFlag) {
    case XINPUT_GAMEPAD_A:
        return GamepadButton::A;
    case XINPUT_GAMEPAD_B:
        return GamepadButton::B;
    case XINPUT_GAMEPAD_X:
        return GamepadButton::X;
    case XINPUT_GAMEPAD_Y:
        return GamepadButton::Y;
    case XINPUT_GAMEPAD_LEFT_SHOULDER:
        return GamepadButton::LB;
    case XINPUT_GAMEPAD_RIGHT_SHOULDER:
        return GamepadButton::RB;
    case XINPUT_GAMEPAD_BACK:
        return GamepadButton::Back;
    case XINPUT_GAMEPAD_START:
        return GamepadButton::Start;
    case XINPUT_GAMEPAD_LEFT_THUMB:
        return GamepadButton::LS;
    case XINPUT_GAMEPAD_RIGHT_THUMB:
        return GamepadButton::RS;
    case XINPUT_GAMEPAD_DPAD_UP:
        return GamepadButton::DPadUp;
    case XINPUT_GAMEPAD_DPAD_DOWN:
        return GamepadButton::DPadDown;
    case XINPUT_GAMEPAD_DPAD_LEFT:
        return GamepadButton::DPadLeft;
    case XINPUT_GAMEPAD_DPAD_RIGHT:
        return GamepadButton::DPadRight;
    default:
        return GamepadButton::COUNT;
    }
}