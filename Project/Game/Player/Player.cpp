#include "Player.h"

/// Enemy
#include "Enemy/CollisionBox/EnemyAttackCollisionBox.h"
#include "Enemy/Types/BaseEnemy.h"
// DeathTimer
#include "DeathTimer/DeathTimer.h"

/// Behavior
#include "Behavior/ComboAttackBehavior/ComboAttackRoot.h"
#include "Behavior/PlayerBehavior/PlayerDeath.h"
#include "Behavior/PlayerBehavior/PlayerMove.h"
#include "Behavior/PlayerBehavior/PlayerSpawn.h"
#include "Behavior/TitleBehavior/TitleFirstFall.h"

// light
#include "Light/AmbientLight.h"
#include "Light/Light.h"
// Field
#include "Field/Field.h"
// LockOn
#include "LockOn/LockOnController.h"
// ComboCreator
#include "ComboCreator/PlayerComboAttackController.h"
// input
#include "input/Input.h"
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
    effects_ = std::make_unique<PlayerEffects>();
    effects_->Init(&baseTransform_);

    ///* グローバルパラメータ
    parameters_ = std::make_unique<PlayerParameter>();
    parameters_->Init();

    ///* 武器生成
    leftHand_  = std::make_unique<PlayerHandLeft>();
    rightHand_ = std::make_unique<PlayerHandRight>();

    // Playerのモデル
    obj3d_.reset(KetaEngine::Object3d::CreateModel("Player.obj"));
    obj3d_->GetModelMaterial()->GetMaterialData()->enableLighting         = 7;
    obj3d_->GetModelMaterial()->GetMaterialData()->environmentCoefficient = 0.05f;

    // Playerの攻撃クラス
    playerCollisionInfo_ = std::make_unique<PlayerAttackCollisionBox>();
    playerCollisionInfo_->Init();
    playerCollisionInfo_->SetPlayerBaseTransform(&baseTransform_);
    playerCollisionInfo_->SetParentTransform(&baseTransform_);

    // トランスフォーム初期化
    obj3d_->transform_.Init();
    leftHand_->Init();
    rightHand_->Init();

    // ペアレント
    obj3d_->transform_.SetParent(&baseTransform_);
    leftHand_->SetParent(&baseTransform_);
    rightHand_->SetParent(&baseTransform_);

    // JumpAttackUI
    jumpAttackUI_ = std::make_unique<JumpAttackUI>();
    jumpAttackUI_->Init();

    // パラメータセット
    baseTransform_.translation_ = parameters_->GetParameters().startPos_;

    /// 通常モードから
    ChangeBehavior(std::make_unique<PlayerSpawn>(this));
    ChangeComboBehavior(std::make_unique<ComboAttackRoot>(this));
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
    if (IsAbleBehavior()) {
        behavior_->Update();
    }

    jumpAttackUI_->Update(GetWorldPosition(), *viewProjection_);

    /// Particle
    effects_->Update(GetWorldPosition());

    // ダメージクールダウン更新
    if (isDamageColling_) {
        damageCollTime_ -= KetaEngine::Frame::DeltaTimeRate();
        if (damageCollTime_ <= 0.0f) {
            isDamageColling_ = false;
        }
    }

    // コンボ更新
    if (!dynamic_cast<PlayerDeath*>(behavior_.get())) {
        comboBehavior_->Update(comboAttackController_->GetRealAttackSpeed(KetaEngine::Frame::DeltaTimeRate()));
    }

    // 死亡モード変更
    if (isDeath_ && *isDeath_) {
        ChangeDeathMode();
    }

    // 移動制限
    MoveToLimit();
    UpdateMatrix();
}

void Player::ChangeDeathMode() {

    if (!dynamic_cast<PlayerDeath*>(behavior_.get())) {
        ChangeBehavior(std::make_unique<PlayerDeath>(this));
    }
}

// タイトル更新
void Player::TitleUpdate() {

    /// Particle
    effects_->Update(GetWorldPosition());

    titleBehavior_->Update();
    /// 行列更新
    UpdateMatrix();
}

void Player::GameIntroUpdate() {

    effects_->Update(GetWorldPosition());

    if (dynamic_cast<PlayerSpawn*>(behavior_.get())) {
        behavior_->Update();
    }
    /// 行列更新
    UpdateMatrix();
}

///=========================================================
/// 移動の入力処理
///==========================================================
Vector3 Player::GetInputDirection() {

    Vector3 velocity         = {0.0f, 0.0f, 0.0f};
    KetaEngine::Input* input = KetaEngine::Input::GetInstance();

    // キーボード入力
    if (input->PushKey(KeyboardKey::W)) {
        velocity.z += 1.0f; // 前進
    }
    if (input->PushKey(KeyboardKey::S)) {
        velocity.z -= 1.0f; // 後退
    }
    if (input->PushKey(KeyboardKey::A)) {
        velocity.x -= 1.0f; // 左移動
    }
    if (input->PushKey(KeyboardKey::D)) {
        velocity.x += 1.0f; // 右移動
    }

    // ジョイスティック入力
    Vector2 stickInput = KetaEngine::Input::GetPadStick(0, 0);
    velocity.x += stickInput.x;
    velocity.z += stickInput.y;

    return velocity;
}

///=========================================================
/// 移動
///==========================================================
void Player::Move(float speed) {

    /// Inputから速度代入
    direction_ = GetInputDirection();

    /// 移動処理
    if (CheckIsMoving()) {
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
        if (dynamic_cast<ComboAttackRoot*>(comboBehavior_.get())) {
            AdaptRotate(); // 回転適応
        }
        FaceToTarget();
    } else {
        FaceToTarget();
    }
}

void Player::FaceToTarget() {
    if (pLockOn_ && pLockOn_->GetLockOn()->GetIsCurrentTarget()) {
        Vector3 differentialVector = pLockOn_->GetLockOn()->GetCurrentTargetPosition() - GetWorldPosition();

        // Y軸周り角度(θy)
        baseTransform_.rotation_.y = std::atan2(differentialVector.x, differentialVector.z);
    }
}

void Player::AdaptRotate() {
    baseTransform_.rotation_.y = LerpShortAngle(baseTransform_.rotation_.y, objectiveAngle_, 0.3f);
}

///=========================================================
/// 動いているか
///==========================================================
bool Player::CheckIsMoving() {
    KetaEngine::Input* input   = KetaEngine::Input::GetInstance();
    bool isMoving              = false;
    const float thresholdValue = 0.3f;
    Vector3 keyboardVelocity   = {0.0f, 0.0f, 0.0f};

    //---------------------------------------------------------------------
    // keyboard
    //---------------------------------------------------------------------
    if (input->PushKey(KeyboardKey::W) || input->PushKey(KeyboardKey::A) || input->PushKey(KeyboardKey::S) || input->PushKey(KeyboardKey::D)) {
        // キーボード入力
        if (input->PushKey(KeyboardKey::W)) {
            keyboardVelocity.z += 1.0f; // 前進
        }
        if (input->PushKey(KeyboardKey::S)) {
            keyboardVelocity.z -= 1.0f; // 後退
        }
        if (input->PushKey(KeyboardKey::A)) {
            keyboardVelocity.x -= 1.0f; // 左移動
        }
        if (input->PushKey(KeyboardKey::D)) {
            keyboardVelocity.x += 1.0f; // 右移動
        }

        if (keyboardVelocity.Length() > 0) {
            isMoving = true;
        }
    } else {
        //----------------------------------------------------------
        // JoyStick
        //----------------------------------------------------------
        Vector2 stickInput = KetaEngine::Input::GetPadStick(0, 0);

        if (stickInput.Length() > thresholdValue) {
            isMoving = true;
        }
    }

    return isMoving;
}

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
void Player::Fall(float& speed, float fallSpeedLimit, float gravity, const bool& isJump) {

    if (!isJump) {
        // 移動
        baseTransform_.translation_.y += speed * KetaEngine::Frame::DeltaTimeRate();
    }

    // 加速する
    speed = max(speed - (gravity * KetaEngine::Frame::DeltaTimeRate()), -fallSpeedLimit);

    // 着地
    if (baseTransform_.translation_.y <= parameters_->GetParameters().startPos_.y) {

        speed                         = 0.0f;
        baseTransform_.translation_.y = parameters_->GetParameters().startPos_.y;
    }
}

///=========================================================
/// パラメータ調整
///==========================================================
void Player::AdjustParam() {
#ifdef _DEBUG

    // プレイヤーのパラメータ
    parameters_->AdjustParam();

    // パーツのパラメータ
    leftHand_->AdjustParam();
    rightHand_->AdjustParam();
    jumpAttackUI_->AdjustParam();

#endif // _DEBUG
}

///==============================================================================
/// 振る舞い切り替え
///===============================================================================
void Player::ChangeBehavior(std::unique_ptr<BasePlayerBehavior> behavior) {
    // 引数で受け取った状態を次の状態としてセット
    behavior_ = std::move(behavior);
}
void Player::ChangeComboBehavior(std::unique_ptr<BaseComboAttackBehavior> behavior) {
    // 引数で受け取った状態を次の状態としてセット
    comboBehavior_ = std::move(behavior);
}

void Player::ChangeTitleBehavior(std::unique_ptr<BaseTitleBehavior> behavior) {
    titleBehavior_ = std::move(behavior);
}
void Player::ChangeCombBoRoot() {
    ChangeComboBehavior(std::make_unique<ComboAttackRoot>(this));
}

/// =========================================================================================
/// setter method
/// =========================================================================================

void Player::SetTitleBehavior() {
    ChangeTitleBehavior(std::make_unique<TitleFirstFall>(this));
}

void Player::UpdateMatrix() {
    /// 行列更新
    leftHand_->Update();
    rightHand_->Update();
    BaseObject::Update();
}

///==============================================================================
/// Collision
///===============================================================================

void Player::OnCollisionStay([[maybe_unused]] BaseCollider* other) {

    // 敵の攻撃コリジョンとの衝突判定
    if (EnemyAttackCollisionBox* attackBox = dynamic_cast<EnemyAttackCollisionBox*>(other)) {
        // ダメージクールダウン中はスキップ
        if (!isDamageColling_ && pDeathTimer_) {
            // ダメージを受ける
            pDeathTimer_->TakeDamage(attackBox->GetAttackValue());
            // クールダウン開始
            isDamageColling_ = true;
            damageCollTime_ = damageCollDuration_;
        }
        return;
    }

    if (BaseEnemy* enemy = dynamic_cast<BaseEnemy*>(other)) {
        // 敵が攻撃中は押し戻し無効
        if (enemy->IsAttacking()) {
            return;
        }

        // 敵の中心座標を取得
        const Vector3& enemyPosition = enemy->GetCollisionPos();

        // プレイヤーと敵の位置の差分ベクトルを計算
        Vector3 delta = baseTransform_.translation_ - enemyPosition;

        // スケール取得
        Vector3 enemyScale = enemy->GetCollisonScale();
        Vector3 myScale    = GetCollisonScale();

        // 押し出す距離の計算
        float pushDistanceX = (enemyScale.x + myScale.x) / 2.0f + 0.1f;
        float pushDistanceZ = (enemyScale.z + myScale.z) / 2.0f + 0.1f;

        // 実際の押し戻し距離を計算
        float pushAmountX = pushDistanceX - std::abs(delta.x);
        float pushAmountZ = pushDistanceZ - std::abs(delta.z);

        // ワープを防ぐために0以下の値を無効化
        pushAmountX = max(0.0f, pushAmountX);
        pushAmountZ = max(0.0f, pushAmountZ);

        // 押し戻し方向
        Vector3 pushDirection = {0, 0, 0};
        float pushDistance    = 0.0f;

        if (pushAmountX > 0.0f && pushAmountZ > 0.0f) {
            // XとZ両方めり込んでいる場合
            if (pushAmountX > pushAmountZ) {
                pushDistance  = pushAmountX;
                pushDirection = {delta.x > 0 ? 1.0f : -1.0f, 0, 0};
            } else {
                pushDistance  = pushAmountZ;
                pushDirection = {0, 0, delta.z > 0 ? 1.0f : -1.0f};
            }
            /// それぞれ片方ずるめり込んでいる
        } else if (pushAmountX > 0.0f) {
            pushDistance  = pushAmountX;
            pushDirection = {delta.x > 0 ? 1.0f : -1.0f, 0, 0};
        } else if (pushAmountZ > 0.0f) {
            pushDistance  = pushAmountZ;
            pushDirection = {0, 0, delta.z > 0 ? 1.0f : -1.0f};
        }

        // ワープを防ぐため、最大移動量を制限
        float MAX_PUSH_DISTANCE = 0.5f;
        pushDistance            = std::min(pushDistance, MAX_PUSH_DISTANCE);

        // 実際に押し戻す
        if (pushDistance > 0.0f) {
            baseTransform_.translation_ += pushDirection * pushDistance;
        }
    }
}

void Player::DissolveUpdate(float dissolve) {
    obj3d_->GetModelMaterial()->GetMaterialData()->dissolveEdgeColor = Vector3(0.6706f, 0.8824f, 0.9804f);
    obj3d_->GetModelMaterial()->GetMaterialData()->dissolveEdgeWidth = 0.09f;
    obj3d_->GetModelMaterial()->GetMaterialData()->enableDissolve    = true;
    obj3d_->GetModelMaterial()->GetMaterialData()->dissolveThreshold = dissolve;
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
    if (dynamic_cast<ComboAttackRoot*>(comboBehavior_.get())) {
        KetaEngine::Light::GetInstance()->GetAmbientLight()->SetIntensity(0.0f);
    } else {
        KetaEngine::Light::GetInstance()->GetAmbientLight()->SetIntensity(0.9f);
    }
}

void Player::MainHeadAnimationStart(const std::string& name) {
    baseTransform_.PlayObjEaseAnimation(name, "MainHead");
}

bool Player::IsDashing() const {
    if (auto* moveState = dynamic_cast<PlayerMove*>(behavior_.get())) {
        return moveState->IsDashing();
    }
    return false;
}

void Player::RotateReset() {
    obj3d_->transform_.rotation_      = {0, 0, 0};
    obj3d_->transform_.translation_.y = 0.0f;
}

void Player::ResetPositionY() {
    baseTransform_.translation_.y = parameters_->GetParameters().startPos_.y;
}

void Player::ResetHeadScale() {
    obj3d_->transform_.scale_ = Vector3::OneVector();
}

bool Player::IsAbleBehavior() {
    bool isAttackRoot = (dynamic_cast<ComboAttackRoot*>(comboBehavior_.get()));
    return (isAttackRoot);
}

void Player::InitInGameScene() {
    Init();

    DissolveUpdate(1.0f);
    leftHand_->DissolveAdapt(1.0f);
    rightHand_->DissolveAdapt(1.0f);
    leftHand_->SetIsEmit(false);
    rightHand_->SetIsEmit(false);
    SetShadowFrag(false);
}

void Player::SetShadowFrag(const bool& isShadow) {
    obj3d_->SetIsShadow(isShadow);
    leftHand_->SetIsShadow(isShadow);
    rightHand_->SetIsShadow(isShadow);
}

///==============================================================================
/// Class Set
///===============================================================================

void Player::SetLockOn(LockOnController* lockOn) {
    pLockOn_ = lockOn;
}

void Player::SetCombo(Combo* combo) {
    pCombo_ = combo;
    comboAttackController_->SetCombo(pCombo_);
}

void Player::SetGameCamera(GameCamera* gameCamera) {
    pGameCamera_ = gameCamera;
}

void Player::SetHitStop(AttackEffect* hitStop) {
    pAttackEffect_ = hitStop;
}

void Player::SetViewProjection(const KetaEngine::ViewProjection* viewProjection) {
    viewProjection_ = viewProjection;
}

void Player::SetComboAttackController(PlayerComboAttackController* playerComboAttackController) {
    comboAttackController_ = playerComboAttackController;
}

bool Player::CheckIsChargeMax() const {
    return currentUpperChargeTime_ >= parameters_->GetParameters().upperParam.chargeTime;
}
