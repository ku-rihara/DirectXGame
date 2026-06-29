#include "PlayerAnimator.h"
#include "3D/Object3D/Object3d.h"
#include "3D/WorldTransform.h"
#include "Frame/Frame.h"
#include "Player/Components/Parts/PlayerHandLeft.h"
#include "Player/Components/Parts/PlayerHandRight.h"

void PlayerAnimator::Init(
    KetaEngine::Object3d* obj3d,
    PlayerHandLeft* leftHand,
    PlayerHandRight* rightHand,
    KetaEngine::WorldTransform* baseTransform) {

    // メンバ変数にポインタをセット
    obj3d_         = obj3d;
    leftHand_      = leftHand;
    rightHand_     = rightHand;
    baseTransform_ = baseTransform;

    // 初期位置保存
    initYPos_ = baseTransform_->translation_.y;

    // ディゾルブエッジ設定
    obj3d_->GetModelMaterial()->GetMaterialData()->dissolveEdgeColor = Vector3(0.6706f, 0.8824f, 0.9804f);
    obj3d_->GetModelMaterial()->GetMaterialData()->dissolveEdgeWidth = 0.09f;
    dissolvePlayer_.Init();

    // MainHead トレイル初期化
    mainHeadTrailPlayer_.Init();
    mainHeadTrailPlayer_.SetFollowPosition(&headTrailFollowPos_);
}

void PlayerAnimator::Update() {

    // Dissolve 更新
    dissolvePlayer_.Update();
    if (dissolvePlayer_.IsPlaying()) {
        dissolvePlayer_.ApplyToMaterial(*obj3d_->GetModelMaterial());
    }

    // MainHead トレイル更新
    if (baseTransform_) {
        headTrailFollowPos_ = baseTransform_->GetWorldPos();
    }
    mainHeadTrailPlayer_.Update();
}

///=============================================================
/// 移動アニメーション再生
///=============================================================
void PlayerAnimator::PlayMoveAnimation() {
    baseTransform_->PlayObjEaseAnimation("Moving", "Player");
}

///=============================================================
/// 待機アニメーション再生
///=============================================================
void PlayerAnimator::PlayWaitAnimation() {
    baseTransform_->PlayObjEaseAnimation("WaitingMotion", "Player");
}

///=============================================================
/// 移動・待機アニメーション停止
///=============================================================
void PlayerAnimator::StopMoveAnimation() {
    baseTransform_->StopObjEaseAnimation();
    obj3d_->transform_.translation_.y = 0.0f;
    baseTransform_->translation_.y    = initYPos_;
}

///=============================================================
/// ジャンプアニメーション再生
///=============================================================
void PlayerAnimator::PlayJumpAnimation() {
    baseTransform_->PlayObjEaseAnimation("JumpRotation", "Player");
}

///=============================================================
/// Dissolve
///=============================================================
void PlayerAnimator::PlayDissolve(const std::string& name) {
    dissolvePlayer_.Play(name);
    leftHand_->PlayDissolve(name);
    rightHand_->PlayDissolve(name);
}

bool PlayerAnimator::IsDissolveFinished() const {
    return dissolvePlayer_.IsFinished();
}

void PlayerAnimator::SetInitialDissolveHidden() {
    obj3d_->GetModelMaterial()->GetMaterialData()->enableDissolve    = true;
    obj3d_->GetModelMaterial()->GetMaterialData()->dissolveThreshold = 1.0f;
}

///=============================================================
/// ダッシュアニメーション
///=============================================================
void PlayerAnimator::PlayDashStartAnimation() {
    obj3d_->transform_.translation_.y = 0.0f;
    baseTransform_->translation_.y    = initYPos_;
    baseTransform_->UpdateMatrix();
    obj3d_->transform_.PlayObjEaseAnimation("DashStart", "Player");
}

bool PlayerAnimator::IsDashStartAnimationFinished() const {
    auto* player = baseTransform_->GetObjEaseAnimationPlayer();
    return player && player->IsFinished();
}

void PlayerAnimator::PlayPlayerCategoryAnimation(const std::string& name) {
    if (!name.empty()) {
        baseTransform_->PlayObjEaseAnimation(name, "Player");
    }
}

///=============================================================
/// ObjEase アニメーション
///=============================================================
void PlayerAnimator::PlayMainHeadAnimation(const std::string& name) {
    baseTransform_->PlayObjEaseAnimation(name, "MainHead");
}

void PlayerAnimator::StartMainHeadTrailEmit(const std::string& presetName, const std::string& category) {
    mainHeadTrailPlayer_.Play(presetName, category);
}

void PlayerAnimator::StopMainHeadTrailEmit() {
    mainHeadTrailPlayer_.StopEmit();
}

void PlayerAnimator::PlayTitleBodyAnimation(const std::string& name) {
    baseTransform_->PlayObjEaseAnimation(name, "TitlePlayer");
}

void PlayerAnimator::PlayTitleRightHandAnimation(const std::string& name) {
    rightHand_->GetObject3D()->transform_.PlayObjEaseAnimation(name, "TitlePlayer");
}

void PlayerAnimator::PlayTitleLeftHandAnimation(const std::string& name) {
    leftHand_->GetObject3D()->transform_.PlayObjEaseAnimation(name, "TitlePlayer");
}

///=============================================================
/// アニメーション終了フラグ
///=============================================================
bool PlayerAnimator::IsTitleBodyAnimationFinished() const {
    auto* player = baseTransform_->GetObjEaseAnimationPlayer();
    return player && player->IsFinished();
}

bool PlayerAnimator::IsTitleRightHandAnimationFinished() const {
    auto* player = rightHand_->GetObject3D()->transform_.GetObjEaseAnimationPlayer();
    return player && player->IsFinished();
}

bool PlayerAnimator::IsTitleLeftHandAnimationFinished() const {
    auto* player = leftHand_->GetObject3D()->transform_.GetObjEaseAnimationPlayer();
    return player && player->IsFinished();
}

bool PlayerAnimator::IsObj3DAnimationFinished() const {
    auto* player = obj3d_->transform_.GetObjEaseAnimationPlayer();
    return player && player->IsFinished();
}

///=============================================================
/// Reset/Setter 
///=============================================================
void PlayerAnimator::ResetHeadScale() {
    obj3d_->transform_.scale_ = Vector3::OneVector();
}

void PlayerAnimator::ResetPositionY() {
    baseTransform_->translation_.y = initYPos_;
}

void PlayerAnimator::RotateReset() {
    obj3d_->transform_.rotation_      = Vector3::ZeroVector();
    obj3d_->transform_.translation_.y = 0.0f;
}

void PlayerAnimator::SetHeadScale(const Vector3& scale) { obj3d_->transform_.scale_ = scale; }
void PlayerAnimator::SetHeadPosY(float posY) { obj3d_->transform_.translation_.y = posY; }
void PlayerAnimator::SetHeadRotateX(float rotX) { obj3d_->transform_.rotation_.x = rotX; }
void PlayerAnimator::SetHeadRotateY(float rotY) { obj3d_->transform_.rotation_.y = rotY; }
