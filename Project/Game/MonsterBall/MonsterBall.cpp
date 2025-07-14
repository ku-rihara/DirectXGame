#include "MonsterBall.h"
#include "assert.h"
#include "Frame/Frame.h"
#include "input/input.h"
#include "Lighrt/Light.h"
#include <imgui.h>

MonsterBall::MonsterBall() {}

MonsterBall::~MonsterBall() {}

void MonsterBall::Init() {

    transform_.Init();
    obj3D_.reset(Object3d::CreateModel("suzanne.obj"));

    modelAnimation_ = std::make_unique<ModelAnimation>();
    modelAnimation_->Create("WalkAnimation.gltf");
    modelAnimation_->Add("Walk.gltf");

    modelAnimation2_ = std::make_unique<ModelAnimation>();
    modelAnimation2_->Create("Walk.gltf");

    modelAnimation_->transform_.translation_.y = -5.0f;
    modelAnimation_->transform_.translation_.z = -14.0f;
    modelAnimation_->transform_.scale_         = {1, 1, 1};

    modelAnimation2_->transform_.translation_.y = -5.0f;
    modelAnimation2_->transform_.translation_.z = -14.0f;
    modelAnimation2_->transform_.scale_         = {1, 1, 1};

    emitter_.reset(ParticleEmitter::CreateParticlePrimitive("jointTest", PrimitiveType::Plane, 300));
    emitter_->SetParentJoint(modelAnimation_.get(), "mixamorig:RightHand");
    // イージングセッティング
    easing_.SetAdaptValue(&transform_.scale_);

    transform_.SetParentJoint(modelAnimation_.get(), "mixamorig:LeftHand");

    transform_.scale_={33, 33, 33};
    modelAnimation_->transform_.rotation_.y = 3.14f;

}

void MonsterBall::Update() {

    modelAnimation_->Update(Frame::DeltaTime());

    Move(55.0f);

    emitter_->Update();
    emitter_->Emit();
    emitter_->EditorUpdate();

    transform_.UpdateMatrix();

       if (!((Input::IsTriggerPad(0, XINPUT_GAMEPAD_A)))) {

        return;
    }
       modelAnimation_->ChangeAnimation("Walk");
}

void MonsterBall::Draw(ViewProjection& viewProjection) {
    modelAnimation_->Draw(viewProjection);
    modelAnimation_->DebugDraw(viewProjection);

    modelAnimation2_->Draw(viewProjection);
    modelAnimation2_->DebugDraw(viewProjection);

    obj3D_->Draw(transform_, viewProjection);
}

void MonsterBall::Debug() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader("MonsterBall")) {
        ImGui::PushID("MonsterBall");
        ImGui::DragFloat3("Position", &modelAnimation_->transform_.translation_.x, 0.1f);
        ImGui::DragFloat3("R", &modelAnimation_->transform_.rotation_.x, 0.1f);
        ImGui::DragFloat3("Scale", &modelAnimation_->transform_.scale_.x, 0.1f);

        ImGui::DragFloat3("MPosition", &transform_.translation_.x, 0.1f);
        ImGui::DragFloat3("MR", &transform_.rotation_.x, 0.1f);
        ImGui::DragFloat3("MScale", &transform_.scale_.x, 0.1f);
        /*	objct3D_->material_.DebugImGui();*/
        ImGui::PopID();
    }
#endif // _DEBUG
}

void MonsterBall::ScaleEasing() {
    easing_.Update(Frame::DeltaTime());
}

///=========================================================
/// 移動の入力処理
///==========================================================
Vector3 MonsterBall::GetInputDirection() {

    Vector3 velocity = {0.0f, 0.0f, 0.0f};
    Input* input     = Input::GetInstance();

    // キーボード入力
    if (input->PushKey(DIK_W)) {
        velocity.z += 1.0f; // 前進
    }
    if (input->PushKey(DIK_S)) {
        velocity.z -= 1.0f; // 後退
    }
    if (input->PushKey(DIK_A)) {
        velocity.x -= 1.0f; // 左移動
    }
    if (input->PushKey(DIK_D)) {
        velocity.x += 1.0f; // 右移動
    }

    // ジョイスティック入力
    Vector2 stickInput = Input::GetPadStick(0, 0);
    velocity.x += stickInput.x;
    velocity.z += stickInput.y;

    return velocity;
}

///=========================================================
/// 移動
///==========================================================
void MonsterBall::Move(const float& speed) {

    /// Inputから速度代入
    direction_ = GetInputDirection();

    /// 移動処理
    if (GetIsMoving()) {
        // 移動量に速さを反映
        direction_ = Vector3::Normalize(direction_) * (speed)*Frame::DeltaTimeRate();
        // 移動ベクトルをカメラの角度だけ回転する
        Matrix4x4 rotateMatrix = MakeIdentity4x4();
        direction_             = TransformNormal(direction_, rotateMatrix);
        // 移動
        modelAnimation_->transform_.translation_ += direction_;
        // 目標角度
        objectiveAngle_ = std::atan2(direction_.x, direction_.z);
    }
}

///=========================================================
/// 動いているか
///==========================================================
bool MonsterBall::GetIsMoving() {
    Input* input               = Input::GetInstance();
    bool isMoving              = false;
    const float thresholdValue = 0.3f;
    Vector3 keyboardVelocity   = {0.0f, 0.0f, 0.0f};

    //---------------------------------------------------------------------
    // keyboard
    //---------------------------------------------------------------------
    if (input->PushKey(DIK_W) || input->PushKey(DIK_A) || input->PushKey(DIK_S) || input->PushKey(DIK_D)) {
        // キーボード入力
        if (input->PushKey(DIK_W)) {
            keyboardVelocity.z += 1.0f; // 前進
        }
        if (input->PushKey(DIK_S)) {
            keyboardVelocity.z -= 1.0f; // 後退
        }
        if (input->PushKey(DIK_A)) {
            keyboardVelocity.x -= 1.0f; // 左移動
        }
        if (input->PushKey(DIK_D)) {
            keyboardVelocity.x += 1.0f; // 右移動
        }

        if (keyboardVelocity.Length() > 0) {
            isMoving = true;
        }
    } else {
        //----------------------------------------------------------
        // JoyStick
        //----------------------------------------------------------
        Vector2 stickInput = Input::GetPadStick(0, 0);

        if (stickInput.Length() > thresholdValue) {
            isMoving = true;
        }
    }

    return isMoving;
}

///=========================================================
/// 　移動制限
///==========================================================
void MonsterBall::MoveToLimit() {

    // フィールドの中心とスケールを取得
    Vector3 fieldCenter = {0.0f, 0.0f, 0.0f}; // フィールド中心
    Vector3 fieldScale  = {330.0f, 330.0f, 330.0f}; // フィールド中心; // フィールドのスケール

    // プレイヤーのスケールを考慮した半径
    float radiusX = fieldScale.x - modelAnimation_->transform_.scale_.x;
    float radiusZ = fieldScale.z - modelAnimation_->transform_.scale_.z;

    // 現在位置が範囲内かチェック
    bool insideX = std::abs(modelAnimation_->transform_.translation_.x - fieldCenter.x) <= radiusX;
    bool insideZ = std::abs(modelAnimation_->transform_.translation_.z - fieldCenter.z) <= radiusZ;

    ///--------------------------------------------------------------------------------
    /// 範囲外なら戻す
    ///--------------------------------------------------------------------------------

    if (!insideX) { /// X座標
        modelAnimation_->transform_.translation_.x = std::clamp(
            modelAnimation_->transform_.translation_.x,
            fieldCenter.x - radiusX,
            fieldCenter.x + radiusX);
    }

    if (!insideZ) { /// Z座標
        modelAnimation_->transform_.translation_.z = std::clamp(
            modelAnimation_->transform_.translation_.z,
            fieldCenter.z - radiusZ,
            fieldCenter.z + radiusZ);
    }

    // 範囲外の反発処理
    if (!insideX || !insideZ) {
        Vector3 directionToCenter = (fieldCenter - modelAnimation_->transform_.translation_).Normalize();
        modelAnimation_->transform_.translation_.x += directionToCenter.x * 0.1f; // 軽く押し戻す
        modelAnimation_->transform_.translation_.z += directionToCenter.z * 0.1f; // 軽く押し戻す
    }
}

/// ===================================================
///   Jump
/// ===================================================
void MonsterBall::Jump(float& speed, const float& fallSpeedLimit, const float& gravity) {
    // 移動
    modelAnimation_->transform_.translation_.y += speed * Frame::DeltaTimeRate();
    Fall(speed, fallSpeedLimit, gravity, true);
}

///=========================================================
/// 　落ちる
///==========================================================
void MonsterBall::Fall(float& speed, const float& fallSpeedLimit, const float& gravity, const bool& isJump) {

    if (!isJump) {
        // 移動
        modelAnimation_->transform_.translation_.y += speed * Frame::DeltaTimeRate();
    }

    // 加速する
    speed = max(speed - (gravity * Frame::DeltaTimeRate()), -fallSpeedLimit);

    // 着地
    if (modelAnimation_->transform_.translation_.y <= 0) {
        modelAnimation_->transform_.translation_.y = 0;
        speed                     = 0.0f;
    }
}
