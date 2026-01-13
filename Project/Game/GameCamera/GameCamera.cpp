#include "GameCamera.h"
// Function
#include "MathFunction.h"
// math
#include "Matrix4x4.h"
// input
#include "input/Input.h"
// class
#include "LockOn/LockOn.h"
// Behavior
#include "Behavior/CameraFollowBehavior.h"
#include"Behavior/CameraResetBehavior.h"

/// std
#include <imgui.h>

void GameCamera::Init() {
    viewProjection_.Init();

    // グローバルパラメータ
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);

    rendition_ = std::make_unique<CameraRendition>();
    rendition_->Init();
    rendition_->SetGameCamera(this);
    rendition_->SetViewProjection(&viewProjection_);

    // 初期Behaviorを追従に設定
    ChangeBehavior(std::make_unique<CameraFollowBehavior>(this));
}

void GameCamera::Update(float speedRate) {
    rendition_->Update(speedRate);
    shakeOffsetPos_ = rendition_->GetShakeOffset();

    // Behavior更新
    if (behavior_) {
        behavior_->Update();
    }

    // ビュー行列の更新
    viewProjection_.UpdateMatrix();
}

void GameCamera::RotateAdapt() {
    // Y軸の回転補間処理
    RotateYInterpolation(destinationAngleY_);

    // 見下ろし角度の固定
    const float fixedPitchAngle = ToRadian(parameter_.rotate);
    viewProjection_.rotation_.x = fixedPitchAngle;
}

void GameCamera::TranslateAdapt() {
    if (!target_) {
        return;
    }

    interTarget_                 = SLerp(interTarget_, target_->translation_, parameter_.interpolationTime.target);
    Vector3 offset               = OffsetCalc(parameter_.offsetPos);
    viewProjection_.translation_ = interTarget_ + offset;
}

void GameCamera::RotateYInterpolation(float targetAngle) {
    // Y軸の回転補間処理
    viewProjection_.rotation_.y = LerpShortAngle(
        viewProjection_.rotation_.y,
        targetAngle,
        parameter_.interpolationTime.rotateY);
}

Vector3 GameCamera::OffsetCalc(const Vector3& offset) const {
    // カメラの角度から回転行列を計算する
    Matrix4x4 rotateMatrix = MakeRotateYMatrix(viewProjection_.rotation_.y);
    Vector3 resultOffset   = TransformNormal(offset + shakeOffsetPos_, rotateMatrix);
    return resultOffset;
}

void GameCamera::ChangeBehavior(std::unique_ptr<BaseCameraBehavior> behavior) {
    // 引数で受け取った状態を次の状態としてセット
    behavior_ = std::move(behavior);
}

void GameCamera::Reset() {
    // 追従対象がいれば
    if (target_) {
        // ResetBehaviorに切り替え
        ChangeBehavior(std::make_unique<CameraResetBehavior>(this));
    }
}

void GameCamera::RegisterParams() {
    globalParameter_->Regist(groupName_, "firstRotate_", &parameter_.rotate);
    globalParameter_->Regist(groupName_, "firstOffset_", &parameter_.offsetPos);
    globalParameter_->Regist(groupName_, "rotateYSpeed", &parameter_.rotateYSpeed);
    globalParameter_->Regist(groupName_, "interpolationRotateYTime", &parameter_.interpolationTime.rotateY);
    globalParameter_->Regist(groupName_, "interpolationTargetTime", &parameter_.interpolationTime.target);
}

void GameCamera::AdjustParam() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        /// 変数の調整
        ImGui::SliderAngle("カメラ見下ろし角度", &parameter_.rotate, 0, 1000);
        ImGui::DragFloat3("オフセット位置", &parameter_.offsetPos.x, 0.01f);
        ImGui::SeparatorText("回転スピード");
        ImGui::DragFloat("Y回転スピード", &parameter_.rotateYSpeed, 0.01f);
        ImGui::SeparatorText("補間タイム");
        ImGui::DragFloat("Y回転", &parameter_.interpolationTime.rotateY, 0.01f);
        ImGui::DragFloat("ターゲット補間", &parameter_.interpolationTime.target, 0.01f);

        /// セーブとロード
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }
#endif // _DEBUG
}

// ================================= その他のメソッド ================================= //

void GameCamera::GetIsCameraMove() {
    if ((stickInput_).Length() > 0.1f) {
        // カメラが動いている処理
    }
}

Vector3 GameCamera::GetWorldPos() const {
    Vector3 worldPos;
    // ワールド行列の平行移動成分を取得
    worldPos.x = viewProjection_.matView_.m[3][0];
    worldPos.y = viewProjection_.matView_.m[3][1];
    worldPos.z = viewProjection_.matView_.m[3][2];

    return worldPos;
}

void GameCamera::SetTarget(const KetaEngine::WorldTransform* target) {
    target_ = target;
    Reset();
}

void GameCamera::PlayAnimation(const std::string& filename) {
    rendition_->AnimationPlay(filename);
}

void GameCamera::PlayShake(const std::string& filename) {
    rendition_->ShakePlay(filename);
}