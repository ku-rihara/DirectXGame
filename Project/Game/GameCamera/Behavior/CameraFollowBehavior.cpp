#include "CameraFollowBehavior.h"
#include "GameCamera/GameCamera.h"
#include "LockOn/LockOn.h"
#include "Input/Input.h"
#include <cmath>

CameraFollowBehavior::CameraFollowBehavior(GameCamera* owner)
    : BaseCameraBehavior("CameraFollow", owner) {
    isReset_ = false;
}

void CameraFollowBehavior::Update(float time) {
    MoveUpdate(time);

    if (isReset_) {
        pOwner_->Reset();
    }
}

Vector2 CameraFollowBehavior::InputUpdate() {
    // 入力無効時は空ベクトルを返す
    if (!pOwner_->GetIsInputEnabled()) {
        return {0.0f, 0.0f};
    }

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

    // RSボタンでカメラリセット
    if (KetaEngine::Input::IsPressPad(0, GamepadButton::RS)) {
        isReset_ = true;
    }

    return inputVector;
}

void CameraFollowBehavior::MoveUpdate(float time) {
    // 入力更新
    Vector2 inputVector = InputUpdate();

    LockOn* lockOn = pOwner_->GetLockOn();
    const void* currentTarget = nullptr;
    
    if (lockOn && lockOn->GetIsActive() && lockOn->ExistTarget()) {
        currentTarget = lockOn->GetIsCurrentTarget();
    }

    if (currentTarget && pOwner_->GetIsLockOnFollowEnabled()) {
        // ロックオン中は毎フレームターゲットの方向へ向け続ける
        const KetaEngine::WorldTransform* camTarget = pOwner_->GetTarget();
        if (camTarget) {
            Vector3 playerPos  = camTarget->GetWorldPos();
            Vector3 targetPos  = lockOn->GetCurrentTargetPosition();
            Vector3 toTarget   = targetPos - playerPos;
            toTarget.y         = 0.0f;
            // 一定距離未満では向きの更新を止める
            if (toTarget.Length() > pOwner_->GetParameter().lockOnFaceMinDistance) {
                float angle = std::atan2(toTarget.x, toTarget.z);
                pOwner_->SetDestinationAngleY(angle);
            }
        }
    } else if (!currentTarget) {
        // ロックオンしていない間は手動操作を許可する
        if (inputVector.Length() > 0.1f) {
            inputVector = inputVector.Normalize();
            pOwner_->AddDestinationAngleY(inputVector.x * pOwner_->GetParameter().rotateYSpeed * time);
        }
    }
  
    // 位置、回転適用
    pOwner_->TranslateAdapt(); 
    pOwner_->RotateAdapt();    
}

void CameraFollowBehavior::Debug() {
#if defined(_DEBUG) || defined(DEVELOPMENT)

#endif
}