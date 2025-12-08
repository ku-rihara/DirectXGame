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
        switch (currentIndex) {
        // 文字キー A-Z
        case 0:
            target = DIK_A;
            break;
        case 1:
            target = DIK_B;
            break;
        case 2:
            target = DIK_C;
            break;
        case 3:
            target = DIK_D;
            break;
        case 4:
            target = DIK_E;
            break;
        case 5:
            target = DIK_F;
            break;
        case 6:
            target = DIK_G;
            break;
        case 7:
            target = DIK_H;
            break;
        case 8:
            target = DIK_I;
            break;
        case 9:
            target = DIK_J;
            break;
        case 10:
            target = DIK_K;
            break;
        case 11:
            target = DIK_L;
            break;
        case 12:
            target = DIK_M;
            break;
        case 13:
            target = DIK_N;
            break;
        case 14:
            target = DIK_O;
            break;
        case 15:
            target = DIK_P;
            break;
        case 16:
            target = DIK_Q;
            break;
        case 17:
            target = DIK_R;
            break;
        case 18:
            target = DIK_S;
            break;
        case 19:
            target = DIK_T;
            break;
        case 20:
            target = DIK_U;
            break;
        case 21:
            target = DIK_V;
            break;
        case 22:
            target = DIK_W;
            break;
        case 23:
            target = DIK_X;
            break;
        case 24:
            target = DIK_Y;
            break;
        case 25:
            target = DIK_Z;
            break;

        // 数字キー 0-9
        case 26:
            target = DIK_0;
            break;
        case 27:
            target = DIK_1;
            break;
        case 28:
            target = DIK_2;
            break;
        case 29:
            target = DIK_3;
            break;
        case 30:
            target = DIK_4;
            break;
        case 31:
            target = DIK_5;
            break;
        case 32:
            target = DIK_6;
            break;
        case 33:
            target = DIK_7;
            break;
        case 34:
            target = DIK_8;
            break;
        case 35:
            target = DIK_9;
            break;

        // 特殊キー
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
            target = DIK_A;
            break;
        }
    }
}

int32_t GetGamepadButtonIndex(int32_t buttonValue) {
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


int32_t GetKeyboardKeyIndex(int32_t keyValue) {
    switch (keyValue) {
    // 文字キー A-Z
    case DIK_A:
        return 0;
    case DIK_B:
        return 1;
    case DIK_C:
        return 2;
    case DIK_D:
        return 3;
    case DIK_E:
        return 4;
    case DIK_F:
        return 5;
    case DIK_G:
        return 6;
    case DIK_H:
        return 7;
    case DIK_I:
        return 8;
    case DIK_J:
        return 9;
    case DIK_K:
        return 10;
    case DIK_L:
        return 11;
    case DIK_M:
        return 12;
    case DIK_N:
        return 13;
    case DIK_O:
        return 14;
    case DIK_P:
        return 15;
    case DIK_Q:
        return 16;
    case DIK_R:
        return 17;
    case DIK_S:
        return 18;
    case DIK_T:
        return 19;
    case DIK_U:
        return 20;
    case DIK_V:
        return 21;
    case DIK_W:
        return 22;
    case DIK_X:
        return 23;
    case DIK_Y:
        return 24;
    case DIK_Z:
        return 25;

    // 数字キー 0-9
    case DIK_0:
        return 26;
    case DIK_1:
        return 27;
    case DIK_2:
        return 28;
    case DIK_3:
        return 29;
    case DIK_4:
        return 30;
    case DIK_5:
        return 31;
    case DIK_6:
        return 32;
    case DIK_7:
        return 33;
    case DIK_8:
        return 34;
    case DIK_9:
        return 35;

    // 特殊キー
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

GamepadButton FromXInputButtonFlag(int32_t xInputFlag) {
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


int32_t ToDIKCode(KeyboardKey key) {
    switch (key) {
    // 文字キー
    case KeyboardKey::A:
        return DIK_A;
    case KeyboardKey::B:
        return DIK_B;
    case KeyboardKey::C:
        return DIK_C;
    case KeyboardKey::D:
        return DIK_D;
    case KeyboardKey::E:
        return DIK_E;
    case KeyboardKey::F:
        return DIK_F;
    case KeyboardKey::G:
        return DIK_G;
    case KeyboardKey::H:
        return DIK_H;
    case KeyboardKey::I:
        return DIK_I;
    case KeyboardKey::J:
        return DIK_J;
    case KeyboardKey::K:
        return DIK_K;
    case KeyboardKey::L:
        return DIK_L;
    case KeyboardKey::M:
        return DIK_M;
    case KeyboardKey::N:
        return DIK_N;
    case KeyboardKey::O:
        return DIK_O;
    case KeyboardKey::P:
        return DIK_P;
    case KeyboardKey::Q:
        return DIK_Q;
    case KeyboardKey::R:
        return DIK_R;
    case KeyboardKey::S:
        return DIK_S;
    case KeyboardKey::T:
        return DIK_T;
    case KeyboardKey::U:
        return DIK_U;
    case KeyboardKey::V:
        return DIK_V;
    case KeyboardKey::W:
        return DIK_W;
    case KeyboardKey::X:
        return DIK_X;
    case KeyboardKey::Y:
        return DIK_Y;
    case KeyboardKey::Z:
        return DIK_Z;

    // 数字キー
    case KeyboardKey::Key0:
        return DIK_0;
    case KeyboardKey::Key1:
        return DIK_1;
    case KeyboardKey::Key2:
        return DIK_2;
    case KeyboardKey::Key3:
        return DIK_3;
    case KeyboardKey::Key4:
        return DIK_4;
    case KeyboardKey::Key5:
        return DIK_5;
    case KeyboardKey::Key6:
        return DIK_6;
    case KeyboardKey::Key7:
        return DIK_7;
    case KeyboardKey::Key8:
        return DIK_8;
    case KeyboardKey::Key9:
        return DIK_9;

    // 特殊キー
    case KeyboardKey::Space:
        return DIK_SPACE;
    case KeyboardKey::Enter:
        return DIK_RETURN;
    case KeyboardKey::Escape:
        return DIK_ESCAPE;
    case KeyboardKey::Backspace:
        return DIK_BACK;
    case KeyboardKey::Tab:
        return DIK_TAB;
    case KeyboardKey::LeftShift:
        return DIK_LSHIFT;
    case KeyboardKey::RightShift:
        return DIK_RSHIFT;
    case KeyboardKey::LeftCtrl:
        return DIK_LCONTROL;
    case KeyboardKey::RightCtrl:
        return DIK_RCONTROL;
    case KeyboardKey::LeftAlt:
        return DIK_LALT;
    case KeyboardKey::RightAlt:
        return DIK_RALT;

    // 矢印キー
    case KeyboardKey::Up:
        return DIK_UP;
    case KeyboardKey::Down:
        return DIK_DOWN;
    case KeyboardKey::Left:
        return DIK_LEFT;
    case KeyboardKey::Right:
        return DIK_RIGHT;

    default:
        return 0; 
    }
}

KeyboardKey FromDIKCode(int32_t dikCode) {
    switch (dikCode) {
    // 文字キー
    case DIK_A:
        return KeyboardKey::A;
    case DIK_B:
        return KeyboardKey::B;
    case DIK_C:
        return KeyboardKey::C;
    case DIK_D:
        return KeyboardKey::D;
    case DIK_E:
        return KeyboardKey::E;
    case DIK_F:
        return KeyboardKey::F;
    case DIK_G:
        return KeyboardKey::G;
    case DIK_H:
        return KeyboardKey::H;
    case DIK_I:
        return KeyboardKey::I;
    case DIK_J:
        return KeyboardKey::J;
    case DIK_K:
        return KeyboardKey::K;
    case DIK_L:
        return KeyboardKey::L;
    case DIK_M:
        return KeyboardKey::M;
    case DIK_N:
        return KeyboardKey::N;
    case DIK_O:
        return KeyboardKey::O;
    case DIK_P:
        return KeyboardKey::P;
    case DIK_Q:
        return KeyboardKey::Q;
    case DIK_R:
        return KeyboardKey::R;
    case DIK_S:
        return KeyboardKey::S;
    case DIK_T:
        return KeyboardKey::T;
    case DIK_U:
        return KeyboardKey::U;
    case DIK_V:
        return KeyboardKey::V;
    case DIK_W:
        return KeyboardKey::W;
    case DIK_X:
        return KeyboardKey::X;
    case DIK_Y:
        return KeyboardKey::Y;
    case DIK_Z:
        return KeyboardKey::Z;

    // 数字キー
    case DIK_0:
        return KeyboardKey::Key0;
    case DIK_1:
        return KeyboardKey::Key1;
    case DIK_2:
        return KeyboardKey::Key2;
    case DIK_3:
        return KeyboardKey::Key3;
    case DIK_4:
        return KeyboardKey::Key4;
    case DIK_5:
        return KeyboardKey::Key5;
    case DIK_6:
        return KeyboardKey::Key6;
    case DIK_7:
        return KeyboardKey::Key7;
    case DIK_8:
        return KeyboardKey::Key8;
    case DIK_9:
        return KeyboardKey::Key9;

    // 特殊キー
    case DIK_SPACE:
        return KeyboardKey::Space;
    case DIK_RETURN:
        return KeyboardKey::Enter;
    case DIK_ESCAPE:
        return KeyboardKey::Escape;
    case DIK_BACK:
        return KeyboardKey::Backspace;
    case DIK_TAB:
        return KeyboardKey::Tab;
    case DIK_LSHIFT:
        return KeyboardKey::LeftShift;
    case DIK_RSHIFT:
        return KeyboardKey::RightShift;
    case DIK_LCONTROL:
        return KeyboardKey::LeftCtrl;
    case DIK_RCONTROL:
        return KeyboardKey::RightCtrl;
    case DIK_LALT:
        return KeyboardKey::LeftAlt;
    case DIK_RALT:
        return KeyboardKey::RightAlt;

    // 矢印キー
    case DIK_UP:
        return KeyboardKey::Up;
    case DIK_DOWN:
        return KeyboardKey::Down;
    case DIK_LEFT:
        return KeyboardKey::Left;
    case DIK_RIGHT:
        return KeyboardKey::Right;

    default:
        return KeyboardKey::COUNT; 
    }
}