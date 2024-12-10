#include "GameCamera.h"
// Function
#include "Easing.h"
// math
#include "Vector3.h"
// etc
#include "input.h"
//std
#include<numbers>
/// behavior
#include"Game/GameCameraBehavior/GameCameraRoot.h"
#include"Game/GameCameraBehavior/GameCameraBack.h"

void GameCamera::Init() {
    viewprojection_.Initialize();
    /// 振る舞い初期化
    ChangeBehavior(std::make_unique<GameCameraRoot>(this));/// 追っかけ
}

void GameCamera::Update() {
   
    behavior_->Update(); 
    // ビュー行列の更新
    viewprojection_.UpdateMatrix();

    Debug();
}

void GameCamera::RootUpdate() {
    Input* input = Input::GetInstance();
    const float rotateSpeed = 0.08f; // 回転速度

    // 旋回操作: 左右キーでY軸の回転を操作
    stickInput_ = { 0.0f, 0.0f, 0.0f }; // スティック入力相当のベクトルをリセット
    if (input->PushKey(DIK_RIGHT)) {
        stickInput_.x = 1.0f;
    }
    else if (input->PushKey(DIK_LEFT)) {
        stickInput_.x = -1.0f;
    }

    stickInput_ = (stickInput_).Normalize();
    destinationAngleY_ += stickInput_.x * rotateSpeed;

    // リセット操作: Rキーでリセット
    if (input->TriggerKey(DIK_R)) {
        Reset();
    }

    // Y軸の回転補間処理
    viewprojection_.rotation_.y = LerpShortAngle(viewprojection_.rotation_.y, destinationAngleY_, 0.3f);

    // 追従対象がいればその座標に追従
    if (target_) {
        interTarget_ = Lerp(interTarget_, target_->translation_, 1.0f);
        Vector3 offset = OffsetCalc();
        viewprojection_.translation_ = interTarget_ + offset;
    }

    // 見下ろし角度の固定
    const float fixedPitchAngle = 3.0f * std::numbers::pi_v<float> / 180.0f;
    viewprojection_.rotation_.x = fixedPitchAngle;


}

void GameCamera::Reset() {
    // 追従対象があればその位置と角度をリセット
    if (target_) {
        interTarget_ = target_->translation_;
        viewprojection_.rotation_.y = LerpShortAngle(viewprojection_.rotation_.y, target_->rotation_.y, 0.3f);
    }
    destinationAngleY_ = viewprojection_.rotation_.y;
    Vector3 offset = OffsetCalc();
    viewprojection_.translation_ = interTarget_ + offset;
}

Vector3 GameCamera::OffsetCalc() const {
    Vector3 offset = { 0.0f, 7.0f, -24.0f };
    Matrix4x4 rotateMatrix = MakeRotateYMatrix(viewprojection_.rotation_.y);
    offset = TransformNormal(offset, rotateMatrix);
    return offset;
}

void GameCamera::SetTarget(const WorldTransform* target) {
    target_ = target;
    Reset();
}

void GameCamera::CameraBackUpdate() {
   
}

void GameCamera::CameraBackInit() {

}


void GameCamera::Debug() {
#ifdef _DEBUG
    ImGui::DragFloat3("CameraPos", &viewprojection_.translation_.x, 0.01f);
    behavior_->Debug();
#endif // _DEBUG
}

Vector3 GameCamera::GetBaseCenterPosition() const {
    Vector3 worldPos;
    worldPos.x = viewprojection_.matView_.m[3][0];
    worldPos.y = viewprojection_.matView_.m[3][1];
    worldPos.z = viewprojection_.matView_.m[3][2];
    return worldPos;
}

Vector3 GameCamera::GetTargetPos()const {
    return target_->translation_;
}

void GameCamera::ChangeBehavior(std::unique_ptr<BaseGameCameraBehavior>behavior) {
    //引数で受け取った状態を次の状態としてセット
    behavior_ = std::move(behavior);
}


void GameCamera::SetBackToRootFrag() {
    isBackToRootMode_ = true;
}
void GameCamera::SetBehaviorBack() {
    ChangeBehavior(std::make_unique<GameCameraBack>(this));/// 追っかけ
}