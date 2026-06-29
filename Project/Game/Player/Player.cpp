#include "Player.h"
#include "Scene/GameObj.h"

/// Enemy
#include "Enemy/Types/BaseEnemy.h"
// CollisionUtils
#include "Utility/CollisionPush/CollisionPushUtils.h"
// StressGauge
#include "StressGauge/StressGauge.h"

// light
#include "Light/AmbientLight.h"
#include "Light/Light.h"
#include "Light/LightingType.h"
// Field
#include "Field/Field.h"
// LockOn
#include "LockOn/LockOnController.h"
// ComboCreator
#include "ComboCreator/PlayerComboAttackController.h"
// Frame
#include "Frame/Frame.h"
// Math
#include "MathFunction.h"

///=========================================================
/// 　初期化
///==========================================================
void Player::Init() {

    BaseObject::Init();

    // エフェクト初期化
    effects_.Init(&baseTransform_);

    // グローバルパラメータ初期化
    parameters_.Init();

    // Playerのモデル
    obj3d_.reset(KetaEngine::Object3d::CreateModel("Player/Player.obj"));
    obj3d_->GetModelMaterial()->GetMaterialData()->enableLighting         = static_cast<int32_t>(KetaEngine::LightingType::Ambient);
    obj3d_->GetModelMaterial()->GetMaterialData()->environmentCoefficient = 0.05f;

    // Playerの攻撃クラス
    playerCollisionInfo_.Init();
    playerCollisionInfo_.SetPlayerBaseTransform(&baseTransform_);

    // トランスフォーム初期化
    obj3d_->transform_.Init();
    leftHand_.Init();
    rightHand_.Init();

    // ペアレント
    obj3d_->transform_.SetParent(&baseTransform_);
    leftHand_.SetParent(&baseTransform_);
    rightHand_.SetParent(&baseTransform_);

    // パラメータセット
    baseTransform_.SetBaseScale(Vector3::OneVector());
    baseTransform_.translation_ = parameters_.GetParameters().startPos_;

    animator_.Init(obj3d_.get(), &leftHand_, &rightHand_, &baseTransform_);

    behaviors_.Init(this);
    HeadLightSetting();
}

///=========================================================
/// 　更新
///==========================================================
void Player::Update() {
    // 前フレームの座標
    prePos_ = GetWorldPosition();
    // ライト
    HeadLightSetting();

    /// 振る舞い処理
    if (!behaviors_.IsDead()) {
        behaviors_.GetComboBehavior()->Update(context_.comboAttackController->GetRealAttackSpeed(KetaEngine::Frame::DeltaTimeRate()));
    }

    if (IsAbleBehavior()) {
        behaviors_.GetBehavior()->Update();
    }

    // ロックオン対象がいればそちらを向く
    FaceToTarget();

    /// Particle
    effects_.Update(GetWorldPosition(), baseTransform_.rotation_);

    // 死亡モード変更
    if (isDeath_ && *isDeath_) {
        ChangeDeathMode();
    }

    // 死亡時にプレイヤーモデルを非表示にする
    if (isDeathRenditionFinish_) {
        obj3d_->transform_.scale_ = Vector3::ZeroVector();
    } else {
        obj3d_->transform_.scale_ = Vector3::OneVector();
    }

    // 移動制限
    baseTransform_.SetBaseScale(parameters_.GetParameters().baseScale_);
    MoveToLimit();
    UpdateMatrix();
}

void Player::ChangeDeathMode() {
    if (!behaviors_.IsDead()) {
        behaviors_.EnterDeath(this);
    }
}

void Player::GameIntroUpdate(float playSpeed) {

    effects_.Update(GetWorldPosition(), baseTransform_.rotation_);

    if (behaviors_.IsSpawning()) {
        behaviors_.GetBehavior()->Update(playSpeed);
    }
    /// 行列更新
    UpdateMatrix();
}

void Player::Move(float speed) {

    if (behaviors_.IsStartDashing()) {
        return;
    }

    // 移動パラメータを取得
    const auto& moveParameter = parameters_.GetParameters().movement;
    movement_.SetAcceleration(moveParameter.acceleration);
    movement_.SetDeceleration(moveParameter.deceleration);
    movement_.SetTurnBrake(moveParameter.turnBrake);

    // 移動更新
    movement_.Update(input_.GetMoveDirection(), baseTransform_, speed, objectiveAngle_);

    // 回転適用
    if (behaviors_.IsComboRoot()) {
        AdaptRotate();
    }
    FaceToTarget();

    moveSpeed_ = movement_.GetSpeed();
}

void Player::FaceToTarget() {
    if (context_.pLockOn && context_.pLockOn->GetLockOn()->GetIsCurrentTarget()) {
        baseTransform_.rotation_.y = CalcFaceAngleY(
            GetWorldPosition(),
            context_.pLockOn->GetLockOn()->GetCurrentTargetPosition());
    }
}

void Player::AdaptRotate() {
    baseTransform_.rotation_.y = LerpShortAngle(baseTransform_.rotation_.y, objectiveAngle_, 0.3f);
}

void Player::MoveToLimit() {
    movement_.MoveToLimit(baseTransform_, Field::baseScale_);
}

void Player::Jump(float& speed, float fallSpeedLimit, float gravity) {
    movement_.Jump(speed, fallSpeedLimit, gravity, baseTransform_,
        parameters_.GetParameters().startPos_.y);
}

void Player::Fall(float& speed, float fallSpeedLimit, float gravity, bool isJump) {
    movement_.Fall(speed, fallSpeedLimit, gravity, baseTransform_,
        parameters_.GetParameters().startPos_.y, isJump);
}

///=========================================================
/// パラメータ調整
///==========================================================
void Player::AdjustParam() {
#if defined(_DEBUG) || defined(DEVELOPMENT)

    // プレイヤーのパラメータ
    parameters_.AdjustParam();

    // パーツのパラメータ
    leftHand_.AdjustParam();
    rightHand_.AdjustParam();

#endif // _DEBUG
}

///==============================================================================
/// 振る舞い切り替え
///===============================================================================
void Player::ChangeCombBoRoot() {
    behaviors_.ResetComboToRoot(this);
}

/// =========================================================================================
/// setter method
/// =========================================================================================

void Player::UpdateMatrix() {
    /// 行列更新
    leftHand_.Update(isDeathRenditionFinish_);
    rightHand_.Update(isDeathRenditionFinish_);

    animator_.Update();

    BaseObject::Update();
}

///==============================================================================
/// Collision
///===============================================================================

void Player::OnCollisionStay([[maybe_unused]] BaseCollider* other) {

    if (other->IsEnemy()) {
        auto* enemy = static_cast<BaseEnemy*>(other);
        // 敵が攻撃中は押し戻し無効
        if (enemy->IsAttacking()) {
            return;
        }

        CollisionPushUtils::ApplyAABBSpherePush(
            baseTransform_.translation_,
            enemy->GetCollisionPos(),
            GetCollisonScale(),
            enemy->GetCollisionRadius());
    }
}

Vector3 Player::GetCollisionPos() const {
    // ローカル座標でのオフセット
    const Vector3 offset = {0.0f, 1.5f, 0.0f};
    // ワールド座標に変換
    Vector3 worldPos = TransformMatrix(offset, baseTransform_.matWorld_);
    return worldPos;
}

/// =======================================================================================
/// SetValue
/// =======================================================================================
void Player::HeadLightSetting() {
    if (behaviors_.IsComboRoot()) {
        KetaEngine::Light::GetInstance()->GetAmbientLight()->SetIntensity(0.0f);
    } else {
        KetaEngine::Light::GetInstance()->GetAmbientLight()->SetIntensity(0.9f);
    }
}

void Player::StartAutoDash() {
    behaviors_.StartAutoDash(this);
}

void Player::ClearAutoDash() {
    behaviors_.ClearForceDash();
}

bool Player::IsAbleBehavior() {
    if (isDeath_ && *isDeath_) {
        return true;
    }
    return behaviors_.IsComboRoot();
}

void Player::InitInGameScene() {
    Init();
    baseTransform_.SetBaseScale(parameters_.GetParameters().baseScale_);

    // 初期状態: スポーンアニメーション開始まで非表示
    animator_.SetInitialDissolveHidden();
    leftHand_.SetInitialDissolveHidden();
    rightHand_.SetInitialDissolveHidden();
    leftHand_.SetIsEmit(false);
    rightHand_.SetIsEmit(false);
    SetShadowFrag(false);
}

void Player::SetShadowFrag(bool isShadow) {
    obj3d_->SetIsShadow(isShadow);
    leftHand_.SetIsShadow(isShadow);
    rightHand_.SetIsShadow(isShadow);
}

///==============================================================================
/// Class Set
///===============================================================================

void Player::SetLockOn(LockOnController* lockOn) {
    context_.pLockOn = lockOn;
}

void Player::SetCombo(Combo* combo) {
    context_.pCombo = combo;
    context_.comboAttackController->SetCombo(context_.pCombo);
}

void Player::SetGameCamera(GameCamera* gameCamera) {
    context_.pGameCamera = gameCamera;
}

void Player::SetViewProjection(const KetaEngine::ViewProjection* viewProjection) {
    viewProjection_ = viewProjection;
    movement_.Init(viewProjection_);
}

void Player::SetComboAttackController(PlayerComboAttackController* playerComboAttackController) {
    context_.comboAttackController = playerComboAttackController;
}

void Player::SetStressGauge(StressGauge* StressGauge) {
    context_.pStressGauge = StressGauge;
}

bool Player::CheckIsChargeMax() const {
    return currentUpperChargeTime_ >= parameters_.GetParameters().upperParam.chargeTime;
}

void Player::Connect(GameObj* go, const KetaEngine::ViewProjection& vp) {
    SetViewProjection(&vp);
    SetLockOn(go->lockOnController_.get());
    SetGameCamera(go->gameCamera_.get());
    SetComboAttackController(go->playerComboAttackController_.get());
    SetCombo(go->combo_.get());
    SetStressGauge(go->StressGauge_.get());
    SetDeathFragPointer(&go->StressGauge_->GetIsDeathRef());
}