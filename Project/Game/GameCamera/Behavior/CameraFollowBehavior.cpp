#include "CameraFollowBehavior.h"
#include "GameCamera/GameCamera.h"
#include "input/Input.h"

CameraFollowBehavior::CameraFollowBehavior(GameCamera* owner)
    : BaseCameraBehavior("CameraFollow", owner) {
    isReset_ = false;
}

void CameraFollowBehavior::Update(float time) {
    time; // 未使用
    MoveUpdate();

    if (isReset_) {
        pOwner_->Reset();
    }
}

Vector2 CameraFollowBehavior::InputUpdate() {
    // 入力処理
    KetaEngine::Input* input = KetaEngine::Input::GetInstance();
    Vector2 inputVector      = {0.0f, 0.0f};

    // ================================= keyBord ================================= //
    if (input->PushKey(KeyboardKey::Right) || input->PushKey(KeyboardKey::Left)) {
        // 右回転
        if (input->PushKey(KeyboardKey::Right)) {
            inputVector.x = 1.0f;
        } else if (input->PushKey(KeyboardKey::Left)) {
            // 左回転
            inputVector.x = -1.0f;
        }
    } else {
        // ================================= GamePad ================================= //
        inputVector = KetaEngine::Input::GetPadStick(0, 1);
    }

    // リセット
    if (input->PushKey(KeyboardKey::R) || KetaEngine::Input::IsPressPad(0, GamepadButton::RS)) {

        isReset_ = true;
    }

    return inputVector;
}

void CameraFollowBehavior::MoveUpdate() {
    // 入力更新
    Vector2 inputVector = InputUpdate();

    //---------------------------入力による回転処理 ---------------------------//
    if (inputVector.Length() > 0.1f) {
        inputVector = inputVector.Normalize();
        pOwner_->AddDestinationAngleY(inputVector.x * pOwner_->GetParameter().rotateYSpeed);
    }

    //--------------------------- 回転、変位の適応 ---------------------------//
    pOwner_->TranslateAdapt(); //< 変位適応
    pOwner_->RotateAdapt(); //< 回転適応
}

void CameraFollowBehavior::Debug() {
#ifdef _DEBUG

#endif
}