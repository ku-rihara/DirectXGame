#include "Player.h"

/// Enemy
#include "Enemy/CollisionBox/EnemyAttackCollisionBox.h"
#include "Enemy/Types/BaseEnemy.h"
// CollisionUtils
#include "utility/CollisionPush/CollisionPushUtils.h"
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

    //* particle
    effects_.Init(&baseTransform_);

    ///* グローバルパラメータ
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
    // コンボ更新を先に行うことで、攻撃入力による状態遷移を優先させる
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

///=========================================================
/// 移動
///==========================================================
void Player::Move(float speed) {

    direction_ = input_.GetMoveDirection();

    /// 移動処理
    if (input_.IsMoving()) {
        // 移動量に速さを反映
        direction_ = direction_.Normalize() * (speed)*KetaEngine::Frame::DeltaTimeRate();
        // 移動ベクトルをカメラの角度だけ回転する
        Matrix4x4 rotateMatrix = MakeRotateYMatrix(viewProjection_->rotation_.y);
        direction_             = TransformNormal(direction_, rotateMatrix);
        // 移動
        baseTransform_.translation_ += direction_;
        // 目標角度
        objectiveAngle_ = std::atan2(direction_.x, direction_.z);
        // 最短角度補間
        if (behaviors_.IsComboRoot()) {
            AdaptRotate();
        }
        FaceToTarget();
    } else {
        FaceToTarget();
    }
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

///=========================================================
/// 動いているか
///==========================================================
///=========================================================
/// 　移動制限
///==========================================================
void Player::MoveToLimit() {

    // フィールドの中心とスケールを取得
    Vector3 fieldCenter = {0.0f, 0.0f, 0.0f}; // フィールド中心
    Vector3 fieldScale  = Field::baseScale_; // フィールドのスケール

    // プレイヤーのスケールを考慮した半径
    float radiusX = fieldScale.x - baseTransform_.scale_.x;
    float radiusZ = fieldScale.z - baseTransform_.scale_.z;

    // 現在位置が範囲内かチェック
    bool insideX = std::abs(baseTransform_.translation_.x - fieldCenter.x) <= radiusX;
    bool insideZ = std::abs(baseTransform_.translation_.z - fieldCenter.z) <= radiusZ;

    ///--------------------------------------------------------------------------------
    /// 範囲外なら戻す
    ///--------------------------------------------------------------------------------

    if (!insideX) { /// X座標
        baseTransform_.translation_.x = std::clamp(
            baseTransform_.translation_.x,
            fieldCenter.x - radiusX,
            fieldCenter.x + radiusX);
    }

    if (!insideZ) { /// Z座標
        baseTransform_.translation_.z = std::clamp(
            baseTransform_.translation_.z,
            fieldCenter.z - radiusZ,
            fieldCenter.z + radiusZ);
    }

    // 範囲外の反発処理
    if (!insideX || !insideZ) {
        Vector3 directionToCenter = (fieldCenter - baseTransform_.translation_).Normalize();
        baseTransform_.translation_.x += directionToCenter.x * 0.1f; // 軽く押し戻す
        baseTransform_.translation_.z += directionToCenter.z * 0.1f; // 軽く押し戻す
    }
}

/// ===================================================
///   Jump
/// ===================================================
void Player::Jump(float& speed, float fallSpeedLimit, float gravity) {
    // 移動
    baseTransform_.translation_.y += speed * KetaEngine::Frame::DeltaTimeRate();
    Fall(speed, fallSpeedLimit, gravity, true);
}

///=========================================================
/// 　落ちる
///==========================================================
void Player::Fall(float& speed, float fallSpeedLimit, float gravity, bool isJump) {

    if (!isJump) {
        // 移動
        baseTransform_.translation_.y += speed * KetaEngine::Frame::DeltaTimeRate();
    }

    // 加速する
    speed = max(speed - (gravity * KetaEngine::Frame::DeltaTimeRate()), -fallSpeedLimit);

    // 着地
    if (baseTransform_.translation_.y <= parameters_.GetParameters().startPos_.y) {

        speed                         = 0.0f;
        baseTransform_.translation_.y = parameters_.GetParameters().startPos_.y;
    }
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

    if (BaseEnemy* enemy = dynamic_cast<BaseEnemy*>(other)) {
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
