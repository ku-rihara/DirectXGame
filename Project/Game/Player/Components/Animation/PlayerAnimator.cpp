#include "PlayerAnimator.h"
#include "3d/Object3D/Object3d.h"
#include "3d/WorldTransform.h"
#include "Frame/Frame.h"
#include "Player/Components/Parts/PlayerHandLeft.h"
#include "Player/Components/Parts/PlayerHandRight.h"

void PlayerAnimator::Init(
    KetaEngine::Object3d* obj3d,
    PlayerHandLeft* leftHand,
    PlayerHandRight* rightHand,
    KetaEngine::WorldTransform* baseTransform) {

    obj3d_         = obj3d;
    leftHand_      = leftHand;
    rightHand_     = rightHand;
    baseTransform_ = baseTransform;

    // ディゾルブエッジ設定
    obj3d_->GetModelMaterial()->GetMaterialData()->dissolveEdgeColor = Vector3(0.6706f, 0.8824f, 0.9804f);
    obj3d_->GetModelMaterial()->GetMaterialData()->dissolveEdgeWidth = 0.09f;
    dissolvePlayer_.Init();
}

void PlayerAnimator::Update() {
    dissolvePlayer_.Update();
    if (dissolvePlayer_.IsPlaying()) {
        dissolvePlayer_.ApplyToMaterial(*obj3d_->GetModelMaterial());
    }
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
    baseTransform_->PlayObjEaseAnimation("Waiting", "Player");
}

///=============================================================
/// 移動・待機アニメーション停止
///=============================================================
void PlayerAnimator::StopMoveAnimation() {
    baseTransform_->StopObjEaseAnimation();
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
    baseTransform_->PlayObjEaseAnimation("DashStart", "Player");
}

bool PlayerAnimator::IsDashStartAnimationFinished() const {
    auto* player = baseTransform_->GetObjEaseAnimationPlayer();
    return player && player->IsFinished();
}

///=============================================================
/// ObjEase アニメーション (タイトル等)
///=============================================================
void PlayerAnimator::PlayMainHeadAnimation(const std::string& name) {
    baseTransform_->PlayObjEaseAnimation(name, "MainHead");
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
