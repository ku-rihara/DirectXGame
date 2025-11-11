#include "GameCamera.h"
// Function
#include "MathFunction.h"
// math
#include "Matrix4x4.h"
// input
#include "input/Input.h"
// class
#include "LockOn/LockOn.h"

/// std
#include <imgui.h>
#include <numbers>

void GameCamera::Init() {
    viewProjection_.Init();

    ///* グローバルパラメータ
    globalParameter_ = GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);

    /*  rotate_ = parameter_.rotate;
      offset_ = parameter_.offsetPos; */

    rendition_ = std::make_unique<CameraRendition>();
    rendition_->Init();
    rendition_->SetGameCamera(this);
    rendition_->SetViewProjection(&viewProjection_);
}

void GameCamera::Update(const float& speedRate) {

    rendition_->Update(speedRate);
    shakeOffsetPos_ = rendition_->GetShakeOffset();

    // カメラの基本移動処理
    MoveUpdate();

    // ビュー行列の更新
    viewProjection_.UpdateMatrix();
}

void GameCamera::MoveUpdate() {

    // 入力処理
    Input* input            = Input::GetInstance();
    const float rotateSpeed = 0.08f;
    Vector2 stickInput      = {0.0f, 0.0f};

    // ================================= keyBord ================================= //
    if (input->PushKey(KeyboardKey::Right) || input->PushKey(KeyboardKey::Left)) {

        // 右回転
        if (input->PushKey(KeyboardKey::Right)) {
            stickInput.x = 1.0f;

        } else if (input->PushKey(KeyboardKey::Left)) {
            // 左回転
            stickInput.x = -1.0f;
        }

    } else {
        // ================================= GamePad ================================= //
        stickInput = Input::GetPadStick(0, 1);
    }

    ///============================================================
    // 入力に対する回転処理
    ///============================================================
    if (stickInput.Length() > 0.1f) {
        stickInput = stickInput.Normalize();
        destinationAngleY_ += stickInput.x * rotateSpeed;
    }

    // reset
    if (input->TriggerKey(KeyboardKey::R) || Input::IsTriggerPad(0, GamepadButton::RS)) {
        Reset();
    }

    // Y軸の回転補間処理
    viewProjection_.rotation_.y = LerpShortAngle(viewProjection_.rotation_.y, destinationAngleY_, 0.3f);

    ///============================================================
    // 回転、変位の適応
    ///============================================================
    TranslateAdapt(); //< 変位適応
    RotateAdapt(); //< 回転適応
}

void GameCamera::RotateAdapt() {
    // Y軸の回転補間処理
    viewProjection_.rotation_.y = LerpShortAngle(viewProjection_.rotation_.y, destinationAngleY_, 0.3f);

    // 見下ろし角度の固定
    const float fixedPitchAngle = parameter_.rotate * std::numbers::pi_v<float> / 180.0f;
    viewProjection_.rotation_.x = fixedPitchAngle;
}

void GameCamera::TranslateAdapt() {
    if (!target_)
        return;
    interTarget_                 = Lerp(interTarget_, target_->translation_, 1.0f);
    Vector3 offset               = OffsetCalc(parameter_.offsetPos);
    viewProjection_.translation_ = interTarget_ + offset;
}

void GameCamera::Reset() {
    // 追従対象がいれば
    if (target_) {
        // 追従座標・角度の初期化
        interTarget_                = target_->translation_;
        viewProjection_.rotation_.y = LerpShortAngle(viewProjection_.rotation_.y, target_->rotation_.y, 0.3f);
    }
    destinationAngleY_ = viewProjection_.rotation_.y;
    // 追従対象からのオフセット
    Vector3 offset               = OffsetCalc(parameter_.offsetPos);
    viewProjection_.translation_ = interTarget_ + offset;
}

Vector3 GameCamera::OffsetCalc(const Vector3& offset) const {
    // カメラの角度から回転行列を計算する
    Matrix4x4 rotateMatrix = MakeRotateYMatrix(viewProjection_.rotation_.y);
    Vector3 resultOffset   = TransformNormal(offset + shakeOffsetPos_, rotateMatrix);
    return resultOffset;
}

void GameCamera::RegisterParams() {
    globalParameter_->Regist(groupName_, "firstRotate_", &parameter_.rotate);
    globalParameter_->Regist(groupName_, "firstOffset_", &parameter_.offsetPos);
}

void GameCamera::AdjustParam() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        /// 変数の調整
        ImGui::SliderAngle("First Rotate", &parameter_.rotate, 0, 1000);
        ImGui::DragFloat3("firstOffset", &parameter_.offsetPos.x, 0.01f);

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

void GameCamera::SetTarget(const WorldTransform* target) {
    target_ = target;
    Reset();
}

void GameCamera::PlayAnimation(const std::string& filename) {
    rendition_->AnimationPlay(filename);
}
void GameCamera::PlayShake(const std::string& filename) {
    rendition_->ShakePlay(filename);
}