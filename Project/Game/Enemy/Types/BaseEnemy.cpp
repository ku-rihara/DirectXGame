#include "BaseEnemy.h"
#include "Enemy/Component/EnemyInitializer.h"

// CollisionUtils
// KillCounter
#include "KillCounter/KillCounter.h"
// Manager
#include "Enemy/EnemyManager/EnemyManager.h"
// behavior
#include "../Behavior/ActionBehavior/CommonBehavior/EnemySpawn.h"
#include "../Behavior/ActionBehavior/CommonBehavior/EnemyWait.h"
#include "../Behavior/DamageReactionBehavior/EnemyDamageReactionRoot.h"
#include "Enemy/Behavior/DamageReactionBehavior/EnemyDeath.h"
// Player
#include "Player/Components/CollisionBox/PlayerAttackCollider.h"
#include "Player/Player.h"
// StressGauge
#include "StressGauge/StressGauge.h"
// Combo
#include "Combo/Combo.h"
// Field
#include "Field/Field.h"
// Camera
#include "GameCamera/GameCamera.h"
// Frame
#include "Frame/Frame.h"
// Math
#include "MathFunction.h"
#include "Matrix4x4.h"

///========================================================
///  初期化
///========================================================
void BaseEnemy::Init(const Vector3& spawnPos) {
    EnemyInitializer(this, spawnPos).Run();
}

///========================================================
/// プール返却前クリーンアップ
///========================================================
void BaseEnemy::PrepareForPool() {
    // ビヘイビアを先に破棄
    behaviorCtrl_.Reset();
    // 残存コールバックを念のためクリア
    animator_.ClearAllCallbacks();
    // UI を非表示にする
    if (enemyUIs_) {
        enemyUIs_->Hide(hp_);
        enemyUIs_->UpdateGroupIcon({}, false);
    }
    // コリジョン無効化
    SetIsAdaptCollision(false);
}

///========================================================
/// 更新
///========================================================
void BaseEnemy::Update() {

    // ビヘイビア更新
    behaviorCtrl_.Update(KetaEngine::Frame::DeltaTimeRate());

    // 攻撃コリジョン更新
    if (attackCollisionBox_) {
        attackCollisionBox_->Update();
        attackCollisionBox_->TimerUpdate(KetaEngine::Frame::DeltaTimeRate());
    }

    // エフェクト更新
    if (enemyEffects_) {
        enemyEffects_->Update(GetWorldPosition());
    }

    // 移動制限
    MoveToLimit();

    // HP割合に応じてボディカラーを更新
    if (colorConfig_ && HPMax_ > 0.0f) {
        float ratio = hp_ / HPMax_;
        Vector3 c   = colorConfig_->GetColor(ratio);
        animator_.SetBodyColor({c.x, c.y, c.z, 1.0f});
    }

    BaseObject::Update();
}

/// ===================================================
///  BaseEnemy Jump
/// ===================================================
void BaseEnemy::Jump(float& speed, float fallSpeedLimit, float gravity) {
    // 移動
    baseTransform_.translation_.y += speed * KetaEngine::Frame::DeltaTimeRate();
    Fall(speed, fallSpeedLimit, gravity, true);
}

///=========================================================
/// 　落ちる
///==========================================================
void BaseEnemy::Fall(float& speed, float fallSpeedLimit, float gravity, bool isJump) {

    if (!isJump) {
        // 移動
        baseTransform_.translation_.y += speed * KetaEngine::Frame::DeltaTimeRate();
    }

    // 加速する
    speed = max(speed - (gravity * KetaEngine::Frame::DeltaTimeRate()), -fallSpeedLimit);
}

///========================================================
/// HpBar表示
///========================================================
void BaseEnemy::DisplaySprite(const KetaEngine::ViewProjection& viewProjection, float distanceToPlayer, bool isOccluded) {
 
    // オクルージョン時はUI非表示
    if (isOccluded) {
        enemyUIs_->Hide(hp_);
        enemyUIs_->UpdateGroupIcon({}, false);
        return;
    }

    Vector3 worldPos = GetWorldPosition();

    const auto& param = baseInfo_.GetParameter();

    // HPバーの位置計算
    Vector3 hpBarWorldPos  = worldPos + Vector3{0.0f, param.hpBarWorldOffsetY, 0.0f};
    Vector2 hpBarScreenPos = ScreenTransform(hpBarWorldPos, viewProjection);

    // グループアイコンの位置計算
    Vector3 iconWorldPos  = worldPos + Vector3{0.0f, param.groupIconWorldOffsetY, 0.0f};
    Vector2 iconScreenPos = ScreenTransform(iconWorldPos, viewProjection);

    // UI更新
    enemyUIs_->Update(hp_, hpBarScreenPos, IsInView(viewProjection));

    //
    if (distanceToPlayer <= baseInfo_.GetManager()->GetHpBarDisplayDistance()) {
        enemyUIs_->SetHPBarOffset(param.hpBarPosOffset);
        enemyUIs_->SetHPGaugeOffset(param.hpGaugePosOffset);
    } else {
        enemyUIs_->Hide(hp_);
    }

    enemyUIs_->UpdateGroupIcon(iconScreenPos, IsInView(viewProjection));
}

Vector3 BaseEnemy::GetDirectionToTarget(const Vector3& target) {

    // 現在のボス位置を取得
    Vector3 enemyPosition = GetWorldPosition();

    // ターゲットへのベクトル
    Vector3 direction = target - enemyPosition;

    return direction;
}

void BaseEnemy::OnCollisionEnter([[maybe_unused]] BaseCollider* other) {
}

void BaseEnemy::OnCollisionStay([[maybe_unused]] BaseCollider* other) {

    if (other->IsPlayerAttack()) {
        auto* attackController = static_cast<PlayerAttackCollider*>(other);
        behaviorCtrl_.OnPlayerAttackCollision(attackController);
        return;
    }
}

void BaseEnemy::MoveToLimit() {

    Vector3 fieldCenter = Vector3::ZeroVector();
    Vector3 fieldScale  = Field::baseScale_;

    float radiusX = fieldScale.x - baseTransform_.scale_.x;
    float radiusZ = fieldScale.z - baseTransform_.scale_.z;

    if (radiusX <= 0.0f || radiusZ <= 0.0f) {
        return;
    }

    baseTransform_.translation_.x = std::clamp(
        baseTransform_.translation_.x,
        fieldCenter.x - radiusX,
        fieldCenter.x + radiusX);

    baseTransform_.translation_.z = std::clamp(
        baseTransform_.translation_.z,
        fieldCenter.z - radiusZ,
        fieldCenter.z + radiusZ);
}

bool BaseEnemy::IsInView(const KetaEngine::ViewProjection& viewProjection) const {

    // 敵のワールド座標
    Vector3 positionWorld = GetWorldPosition();

    // ビュー空間に変換
    Vector3 positionView = TransformMatrix(positionWorld, viewProjection.matView_);

    // ===== 後ろにいる場合は描画しない =====
    if (positionView.z <= 0.0f) {
        return false;
    }

    // ===== スクリーン座標に変換 =====
    Vector2 positionScreen = ScreenTransform(positionWorld, viewProjection);

    // 画面範囲チェック
    if (positionScreen.x < 0.0f || positionScreen.x > 1280.0f || positionScreen.y < 0.0f || positionScreen.y > 720.0f) {
        return false;
    }

    return true;
}

void BaseEnemy::TakeDamage(float damageValue) {

    // ダメージを受ける
    hp_ -= damageValue;

    // コンボをカウント
    if (baseInfo_.GetCombo()) {
        baseInfo_.GetCombo()->ComboCountUP();
    }

    // ダメージコールバック
    if (onDamageTaken_) {
        onDamageTaken_();
    }

    if (hp_ <= 0.0f && !isDeathPending_) {
        // 撃破カウント
        if (baseInfo_.GetKillCounter()) {
            baseInfo_.GetKillCounter()->AddKillCount();
        }
    }

    if (hp_ < 0.0f) {
        hp_ = 0.0f;
    }
}

void BaseEnemy::SetIsDeath(bool is) {
    isDeath_ = is;
    if (is) {
        SetIsAdaptCollision(false);
        animator_.SetAnimationActive(false);
    }
}

std::unique_ptr<BaseEnemyBehavior> BaseEnemy::CreatePostSpawnBehavior() {
    return std::make_unique<EnemyWait>(this);
}

void BaseEnemy::StartDamageColling(float collingTime, const std::string& reactiveAttackName) {
    behaviorCtrl_.StartDamageColling(collingTime, reactiveAttackName);
}

void BaseEnemy::ThrustRenditionInit() {
    enemyEffects_->Emit("EnemyThrust");
}

void BaseEnemy::DeathRenditionInit() {
    enemyEffects_->Emit("EnemyDeath");
}

void BaseEnemy::DirectionToPlayer(bool isOpposite) {
    float objectiveAngle       = CalcFaceAngleY(GetWorldPosition(), baseInfo_.GetPlayer()->GetWorldPosition(), !isOpposite);
    baseTransform_.rotation_.y = LerpShortAngle(baseTransform_.rotation_.y, objectiveAngle, 0.8f);
}


float BaseEnemy::CalcDistanceToPlayer() {
    // プレイヤーへの方向
    Vector3 directionToPlayer = GetDirectionToTarget(baseInfo_.GetPlayer()->GetWorldPosition());
    float distance            = std::sqrt(directionToPlayer.x * directionToPlayer.x + directionToPlayer.z * directionToPlayer.z);
    return distance;
}

Vector3 BaseEnemy::GetCollisionPos() const {
    // パラメータからオフセットを取得
    const Vector3& offset = baseInfo_.GetParameter().collisionOffset;
    // ワールド座標に変換
    Vector3 worldPos = TransformMatrix(offset, baseTransform_.matWorld_);
    return worldPos;
}

void BaseEnemy::ChangeDamageReactionBehavior(std::unique_ptr<BaseEnemyDamageReaction> behavior) {
    behaviorCtrl_.ChangeDamageReactionBehavior(std::move(behavior));
}

void BaseEnemy::ChangeBehavior(std::unique_ptr<BaseEnemyBehavior> behavior) {
    behaviorCtrl_.ChangeBehavior(std::move(behavior));
}

void BaseEnemy::BackToDamageRoot() {
    ResetDamageCooling();
    ChangeDamageReactionBehavior(std::make_unique<EnemyDamageReactionRoot>(this));
    animator_.ResetToWaitAnimation();
}

void BaseEnemy::RefreshCollision() {
    SetCollisionRadius(baseInfo_.GetParameter().collisionRad);
}


bool BaseEnemy::IsInDeathBehavior() const {
    auto* dmg = behaviorCtrl_.GetDamageBehavior();
    return dmg && dmg->IsDeath();
}


void BaseEnemy::ScaleReset() {
    baseTransform_.scale_ = baseInfo_.GetParameter().baseScale_;
}

