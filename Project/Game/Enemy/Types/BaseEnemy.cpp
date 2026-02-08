#include "BaseEnemy.h"

// std
#include <algorithm>
// Manager
#include "Enemy/EnemyManager.h"
// behavior
#include "../Behavior/ActionBehavior/CommonBehavior/EnemySpawn.h"
#include "../Behavior/DamageReactionBehavior/EnemyDamageReactionRoot.h"
#include "Enemy/Behavior/DamageReactionBehavior/EnemyDeath.h"
// Player
#include "Player/CollisionBox/PlayerAttackCollisionBox.h"
#include "Player/ComboCreator/PlayerComboAttackData.h"
#include "Player/Player.h"
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

    // HP
    HPMax_ = 115.0f;
    hp_    = HPMax_;
    hpBar_ = std::make_unique<EnemyHPBar>();
    hpBar_->Init(HPMax_);

    /// transform
    baseTransform_.translation_   = spawnPos;
    baseTransform_.translation_.y = parameter_.basePosY;
    baseTransform_.scale_         = Vector3::ZeroVector();

    /// attack collision
    attackCollisionBox_ = std::make_unique<EnemyAttackCollisionBox>();
    attackCollisionBox_->Init();
    attackCollisionBox_->SetEnemy(this);
    attackCollisionBox_->SetParentTransform(&baseTransform_);

    // エフェクト初期化
    enemyEffects_ = std::make_unique<EnemyEffects>();
    enemyEffects_->Init(&baseTransform_);

    // 振る舞い初期化
    ChangeDamageReactionBehavior(std::make_unique<EnemyDamageReactionRoot>(this));
  
}

///========================================================
/// 更新
///========================================================
void BaseEnemy::Update() {

    if (dynamic_cast<EnemyDamageReactionRoot*>(damageBehavior_.get())) {
        moveBehavior_->Update();
    }

    // ダメージクールタイム更新
    DamageCollingUpdate(KetaEngine::Frame::DeltaTimeRate());

    // ダメージBehavior更新
    damageBehavior_->Update(KetaEngine::Frame::DeltaTimeRate());

    // 攻撃コリジョン更新
    if (attackCollisionBox_) {
        attackCollisionBox_->Update();
        attackCollisionBox_->TimerUpdate(KetaEngine::Frame::DeltaTimeRate());
    }

    // エフェクト更新
    if (enemyEffects_) {
        enemyEffects_->Update(GetWorldPosition());
    }

    MoveToLimit();

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
void BaseEnemy::DisplaySprite(const KetaEngine::ViewProjection& viewProjection) {
    // ワールド座標からスクリーン座標に変換
    Vector2 positionScreen = ScreenTransform(GetWorldPosition(), viewProjection);
    // Vector2に格納
    Vector2 positionScreenV2 = positionScreen - parameter_.hpBarPosOffset;
    // Hpバーの座標確定
    Vector2 hpBarPosition = positionScreenV2;
    // HPBarスプライト
    hpBar_->SetPosition(hpBarPosition);
    // Hpバー更新
    hpBar_->Update(hp_);

    hpBar_->SetIsDraw(IsInView(viewProjection));
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

    if (PlayerAttackCollisionBox* attackController = dynamic_cast<PlayerAttackCollisionBox*>(other)) {
        // プレイヤーとの攻撃コリジョン判定
        ChangeDamageReactionByPlayerAttack(attackController);
        return;
    }

    if (BaseEnemy* enemy = dynamic_cast<BaseEnemy*>(other)) {
        // 攻撃中は押し戻し無効
        if (isAttacking_ || enemy->IsAttacking()) {
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
        float pushDistanceX = (enemyScale.x + myScale.x) / 2.0f + 0.5f;
        float pushDistanceZ = (enemyScale.z + myScale.z) / 2.0f + 0.5f;

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
            /// それぞれ片方とるめり込んでいる
        } else if (pushAmountX > 0.0f) {
            pushDistance  = pushAmountX;
            pushDirection = {delta.x > 0 ? 1.0f : -1.0f, 0, 0};
        } else if (pushAmountZ > 0.0f) {
            pushDistance  = pushAmountZ;
            pushDirection = {0, 0, delta.z > 0 ? 1.0f : -1.0f};
        }

        // ワープを防ぐため、最大移動量を制限
        float MAX_PUSH_DISTANCE = 1.0f;
        pushDistance            = std::min(pushDistance, MAX_PUSH_DISTANCE);

        // 実際に押し戻す
        if (pushDistance > 0.0f) {
            baseTransform_.translation_ += pushDirection * pushDistance;
        }
    }
}

void BaseEnemy::MoveToLimit() {

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

void BaseEnemy::ChangeDamageReactionByPlayerAttack(PlayerAttackCollisionBox* attackController) {

    if (dynamic_cast<EnemyDeath*>(damageBehavior_.get())) {
        return;
    }

    if (!attackController->GetComboAttackData()) {
        return;
    }

    // プレイヤーの攻撃名を取得
    std::string attackName = attackController->GetComboAttackData()->GetGroupName();

    // 攻撃名が空かチェック
    if (attackName.empty()) {
        return;
    }

    // ダメージクーリング中
    if (isDamageColling_ && lastReceivedAttackName_ == attackName) {
        return;
    }

    // Rootにし、受けたダメージの判定を行う
    ChangeDamageReactionBehavior(std::make_unique<EnemyDamageReactionRoot>(this));

    if (EnemyDamageReactionRoot* damageReaction = dynamic_cast<EnemyDamageReactionRoot*>(damageBehavior_.get())) {
        damageReaction->SelectDamageActionBehaviorByAttack(attackController);
    }
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
    pCombo_->ComboCountUP();

    if (hp_ < 0.0f) {
        hp_ = 0.0f;
    }
}

void BaseEnemy::StartDamageColling(float collingTime, const std::string& reactiveAttackName) {
    isDamageColling_        = true;
    lastReceivedAttackName_ = reactiveAttackName;
    damageCollTime_         = collingTime;
}

void BaseEnemy::DamageCollingUpdate(float deltaTime) {
    if (!isDamageColling_) {
        return;
    }

    // クールタイム減算
    damageCollTime_ -= deltaTime;

    if (damageCollTime_ <= 0.0f) {
        isDamageColling_ = false;
    }
}

void BaseEnemy::ThrustRenditionInit() {
    // ガレキパーティクル
    pEnemyManager_->ThrustEmit(GetWorldPosition());
}

void BaseEnemy::DeathRenditionInit() {
    pEnemyManager_->DeathEmit(GetWorldPosition());
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
    // 引数で受け取った状態を次の状態としてセット
    damageBehavior_ = std::move(behavior);
}

void BaseEnemy::ChangeBehavior(std::unique_ptr<BaseEnemyBehavior> behavior) {
    moveBehavior_ = std::move(behavior);
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

void BaseEnemy::BackToDamageRoot() {
    ChangeDamageReactionBehavior(std::make_unique<EnemyDamageReactionRoot>(this));
}

void BaseEnemy::SetParameter(const Type& type, const Parameter& parameter) {
    type_      = type;
    parameter_ = parameter;
    // コリジョンサイズを適用
    SetCollisionScale(parameter_.collisionSize);
}

void BaseEnemy::SetBodyColor(const Vector4& color) {
    if (objAnimation_) {
        objAnimation_->GetModelMaterial()->GetMaterialData()->color = color;
    }
}

void BaseEnemy::RotateInit() {
    if (objAnimation_) {
        objAnimation_->transform_.rotation_ = Vector3::ZeroVector();
    }
}

void BaseEnemy::ScaleReset() {
    baseTransform_.scale_ = parameter_.baseScale_;
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
    if (chaseAnimState_ == ChaseAnimationState::PRE_DASH && isPreDashFinished_) {
        chaseAnimState_ = ChaseAnimationState::DASHING;
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
    chaseAnimState_    = ChaseAnimationState::NONE;
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

void BaseEnemy::DirectionToPlayer(bool isOpposite) {
    
    // プレイヤーへの方向
    Vector3 directionToPlayer = GetDirectionToTarget(pPlayer_->GetWorldPosition());

    if (isOpposite) {
        directionToPlayer *= -1.0f;
    }
    // 正規化
    directionToPlayer.y = 0.0f;
    directionToPlayer.Normalize();

    // 目標角度を計算
    float objectiveAngle = std::atan2(-directionToPlayer.x, -directionToPlayer.z);


    // 最短角度補間で回転を更新
    baseTransform_.rotation_.y = LerpShortAngle(baseTransform_.rotation_.y, objectiveAngle, 0.8f);
}

float BaseEnemy::CalcDistanceToPlayer() {
    // プレイヤーへの方向
    Vector3 directionToPlayer = GetDirectionToTarget(pPlayer_->GetWorldPosition());
    float distance            = std::sqrt(directionToPlayer.x * directionToPlayer.x + directionToPlayer.z * directionToPlayer.z);
    return distance;
}