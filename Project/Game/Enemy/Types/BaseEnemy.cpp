#include "BaseEnemy.h"

// std
#include <algorithm>
// CollisionUtils
#include "utility/CollisionPush/CollisionPushUtils.h"
// KillCounter
#include "KillCounter/KillCounter.h"
// Manager
#include "Enemy/EnemyManager.h"
// behavior
#include "../Behavior/ActionBehavior/CommonBehavior/EnemySpawn.h"
#include "../Behavior/ActionBehavior/CommonBehavior/EnemyWait.h"
#include "../Behavior/DamageReactionBehavior/EnemyDamageReactionRoot.h"
#include "Enemy/Behavior/DamageReactionBehavior/EnemyDeath.h"
// Player
#include "Player/Components/CollisionBox/PlayerAttackCollider.h"
#include "Player/Player.h"
// DeathTimer
#include "DeathTimer/DeathTimer.h"
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

    // プール再利用時のためフラグをリセット
    isDeath_           = false;
    isDeathPending_    = false;
    isCollisionRope_   = false;
    isInAnticipation_  = false;
    isAttacking_       = false;
    chaseAnimeState_   = ChaseAnimationState::NONE;
    isPreDashFinished_ = false;
    damageReactionAnimations_.clear();

    behaviorCtrl_.Init(this);

    // HP
    HPMax_ = parameter_.hpMax;
    hp_    = HPMax_;

    // プール再利用時は既存オブジェクトを使い回す
    if (!enemyUIs_) {
        enemyUIs_ = std::make_unique<EnemyUIs>();
    }
    enemyUIs_->Init(HPMax_);

    /// transform
    baseTransform_.translation_   = spawnPos;
    baseTransform_.translation_.y = parameter_.basePosY;
    baseTransform_.SetAnchorRotation(Vector3(0.0f, 1.5f, 0.0f));
    baseTransform_.scale_ = Vector3::ZeroVector();

    /// attack collision
    if (!attackCollisionBox_) {
        attackCollisionBox_ = std::make_unique<EnemyAttackCollisionBox>();
    }
    attackCollisionBox_->Init();
    attackCollisionBox_->SetEnemy(this);
    attackCollisionBox_->SetParentTransform(&baseTransform_);

    // エフェクト初期化
    if (!enemyEffects_) {
        enemyEffects_ = std::make_unique<EnemyEffects>();
    }
    enemyEffects_->Init(&baseTransform_);

    // 振る舞い初期化
    ChangeDamageReactionBehavior(std::make_unique<EnemyDamageReactionRoot>(this));
}

///========================================================
/// プール返却前クリーンアップ
///========================================================
void BaseEnemy::PrepareForPool() {
    // ビヘイビアを先に破棄
    behaviorCtrl_.Reset();
    // 残存コールバックを念のためクリア
    if (objAnimation_) {
        objAnimation_->ClearAllAnimationEndCallbacks();
    }
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
        SetBodyColor({c.x, c.y, c.z, 1.0f});
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
void BaseEnemy::Fall(float& speed, float fallSpeedLimit, float gravity, const bool& isJump) {

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

    // HPバーの位置計算
    Vector3 hpBarWorldPos  = worldPos + Vector3{0.0f, parameter_.hpBarWorldOffsetY, 0.0f};
    Vector2 hpBarScreenPos = ScreenTransform(hpBarWorldPos, viewProjection);

    // グループアイコンの位置計算
    Vector3 iconWorldPos  = worldPos + Vector3{0.0f, parameter_.groupIconWorldOffsetY, 0.0f};
    Vector2 iconScreenPos = ScreenTransform(iconWorldPos, viewProjection);

    // UI更新
    enemyUIs_->Update(hp_, hpBarScreenPos, IsInView(viewProjection));

    // 
    if (distanceToPlayer <= pEnemyManager_->GetHpBarDisplayDistance()) {
        enemyUIs_->SetHPBarOffset(parameter_.hpBarPosOffset);
        enemyUIs_->SetHPGaugeOffset(parameter_.hpGaugePosOffset);
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

    if (PlayerAttackCollider* attackController = dynamic_cast<PlayerAttackCollider*>(other)) {
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
    if (pCombo_) {
        pCombo_->ComboCountUP();
    }

    // ダメージコールバック
    if (onDamageTaken_) {
        onDamageTaken_();
    }

    if (hp_ <= 0.0f && !isDeathPending_) {
        // 撃破カウント
        if (pKillCounter_) {
            pKillCounter_->AddKillCount();
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
        SetAnimationActive(false);
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
    float objectiveAngle       = CalcFaceAngleY(GetWorldPosition(), pPlayer_->GetWorldPosition(), !isOpposite);
    baseTransform_.rotation_.y = LerpShortAngle(baseTransform_.rotation_.y, objectiveAngle, 0.8f);
}

///========================================================
/// 指定したアニメーションを再生
///========================================================
void BaseEnemy::PlayAnimation(AnimationType type, bool isLoop) {
    if (!objAnimation_) {
        return;
    }

    const std::string& animeName = GetAnimationName(type);
    if (animeName.empty()) {
        return;
    }

    objAnimation_->ChangeAnimation(animeName);
    objAnimation_->SetLoop(isLoop);
}

///========================================================
/// アニメーション名で直接再生
///========================================================
bool BaseEnemy::PlayAnimationByName(const std::string& animationName, bool isLoop) {
    if (!objAnimation_ || animationName.empty()) {
        return false;
    }

    // 利用可能なアニメーションリストを取得して確認
    auto animeNames = objAnimation_->GetAnimationNames();
    auto it         = std::find(animeNames.begin(), animeNames.end(), animationName);

    if (it != animeNames.end()) {
        objAnimation_->ChangeAnimation(animationName);
        objAnimation_->SetLoop(isLoop);
        return true;
    }

    return false; // アニメーションが見つからない
}

///========================================================
/// 追跡中のアニメーション更新
///========================================================
void BaseEnemy::UpdateChaseAnimation([[maybe_unused]] float deltaTime) {
    if (!objAnimation_) {
        return;
    }

    // 予備動作が終了したらダッシュアニメーションに切り替え
    if (chaseAnimeState_ == ChaseAnimationState::PRE_DASH && isPreDashFinished_) {
        chaseAnimeState_ = ChaseAnimationState::DASHING;
        objAnimation_->ChangeAnimation(GetAnimationName(AnimationType::Dash));
        objAnimation_->SetLoop(true); // ダッシュはループ
    }
}

///========================================================
/// 待機アニメーションにリセット
///========================================================
void BaseEnemy::ResetToWaitAnimation() {
    if (!objAnimation_) {
        return;
    }

    // 待機アニメーションに戻す
    chaseAnimeState_   = ChaseAnimationState::NONE;
    isPreDashFinished_ = false;
    objAnimation_->ChangeAnimation(GetAnimationName(AnimationType::Wait));
    objAnimation_->SetLoop(true); // 待機アニメーションはループ
}

std::vector<std::string> BaseEnemy::GetAnimationNames() const {
    if (objAnimation_) {
        return objAnimation_->GetAnimationNames();
    }
    return {};
}

std::string BaseEnemy::GetModelFolder() const {
    return (type_ == Type::NORMAL) ? "NormalEnemy/" : "StrongEnemy/";
}

void BaseEnemy::AddDamageReactionAnimation(const std::string& name, bool isLoop) {
    objAnimation_->Add(GetModelFolder() + name + ".gltf");
    damageReactionAnimations_.push_back({name, isLoop});
}

float BaseEnemy::CalcDistanceToPlayer() {
    // プレイヤーへの方向
    Vector3 directionToPlayer = GetDirectionToTarget(pPlayer_->GetWorldPosition());
    float distance            = std::sqrt(directionToPlayer.x * directionToPlayer.x + directionToPlayer.z * directionToPlayer.z);
    return distance;
}

Vector3 BaseEnemy::GetCollisionPos() const {
    // パラメータからオフセットを取得
    const Vector3& offset = parameter_.collisionOffset;
    // ワールド座標に変換
    Vector3 worldPos = TransformMatrix(offset, baseTransform_.matWorld_);
    return worldPos;
}

void BaseEnemy::SetPlayer(Player* player) {
    pPlayer_ = player;
}

void BaseEnemy::ChangeDamageReactionBehavior(std::unique_ptr<BaseEnemyDamageReaction> behavior) {
    behaviorCtrl_.ChangeDamageReactionBehavior(std::move(behavior));
}

void BaseEnemy::ChangeBehavior(std::unique_ptr<BaseEnemyBehavior> behavior) {
    behaviorCtrl_.ChangeBehavior(std::move(behavior));
}

void BaseEnemy::SetGameCamera(GameCamera* gameCamera) {
    pGameCamera_ = gameCamera;
}

void BaseEnemy::SetManager(EnemyManager* manager) {
    pEnemyManager_ = manager;
}

void BaseEnemy::SetCombo(Combo* manager) {
    pCombo_ = manager;
}

void BaseEnemy::SetKillCounter(KillCounter* killCounter) {
    pKillCounter_ = killCounter;
}

void BaseEnemy::BackToDamageRoot() {
    ResetDamageCooling();
    ChangeDamageReactionBehavior(std::make_unique<EnemyDamageReactionRoot>(this));
    ResetToWaitAnimation();
}

void BaseEnemy::SetParameter(const Type& type, const Parameter& parameter) {
    type_      = type;
    parameter_ = parameter;
    // コリジョンサイズを適用
    SetCollisionRadius(parameter_.collisionRad);
}

void BaseEnemy::SetBodyColor(const Vector4& color) {
    if (objAnimation_) {
        objAnimation_->GetModelMaterial()->GetMaterialData()->color = color;
    }
}

void BaseEnemy::SetAnimationName(AnimationType type, const std::string& name) {

    if (type == AnimationType::Wait) {
        // プール再利用時はモデルを再生成しない
        if (!objAnimation_) {
            objAnimation_.reset(KetaEngine::Object3DAnimation::CreateModel(GetModelFolder() + name + ".gltf"));
            objAnimation_->Init();
        }
        animationNames_[static_cast<size_t>(type)] = name;
        return;
    }

    // 同スロットに既にアニメーションが登録済みなら追加ロードしない
    if (animationNames_[static_cast<size_t>(type)].empty()) {
        objAnimation_->Add(GetModelFolder() + name + ".gltf");
    }
    animationNames_[static_cast<size_t>(type)] = name;
}

bool BaseEnemy::IsInDeathBehavior() const {
    return dynamic_cast<EnemyDeath*>(behaviorCtrl_.GetDamageBehavior()) != nullptr;
}

void BaseEnemy::RotateInit() {
    if (objAnimation_) {
        objAnimation_->transform_.rotation_ = Vector3::ZeroVector();
    }
}

void BaseEnemy::ScaleReset() {
    baseTransform_.scale_ = parameter_.baseScale_;
}

std::vector<std::string> BaseEnemy::GetDamageReactionAnimationNames() const {
    std::vector<std::string> names;
    names.reserve(damageReactionAnimations_.size());
    for (const auto& info : damageReactionAnimations_) {
        names.push_back(info.name);
    }
    return names;
}

bool BaseEnemy::GetDamageReactionAnimationIsLoop(const std::string& name) const {
    for (const auto& info : damageReactionAnimations_) {
        if (info.name == name) {
            return info.isLoop;
        }
    }
    return false;
}