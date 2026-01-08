#include "BaseEnemy.h"

// behavior
#include "../Behavior/DamageReactionBehavior/EnemyDamageReactionAction.h"
#include "../Behavior/DamageReactionBehavior/EnemyDamageReactionRoot.h"
#include "../Behavior/DamageReactionBehavior/EnemyRopeBoundReaction.h"
#include "../Behavior/NormalBehavior/EnemySpawn.h"
#include "Enemy/EnemyManager.h"

#include "Combo/Combo.h"

/// collisionBox
#include "CollisionBox/PlayerCollisionInfo.h"
#include "Player/ComboCreator/PlayerComboAttackData.h"

#include "AttackEffect/AttackEffect.h"
#include "audio/Audio.h"
#include "Enemy/Behavior/DamageReactionBehavior/EnemyDeath.h"
#include "Field/Field.h"
#include "Field/SideRope/SideRope.h"
#include "Frame/Frame.h"
#include "GameCamera/GameCamera.h"
#include "Matrix4x4.h"
#include "Player/Player.h"

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

    /// collision
    collisionBox_ = std::make_unique<EnemyCollisionBox>();
    collisionBox_->Init();
    collisionBox_->SetSize(Vector3(3.2f, 3.2f, 3.2f));

    findSprite_    = std::make_unique<FindSprite>();
    notFindSprite_ = std::make_unique<NotFindSprite>();

    findSprite_->Init();
    notFindSprite_->Init();

    // エフェクト初期化
    enemyEffects_ = std::make_unique<EnemyEffects>();
    enemyEffects_->Init(&baseTransform_);

    // audio
    /*deathSound_  = KetaEngine::Audio::GetInstance()->LoadWave("EnemyDeath.wav");
    thrustSound_ = KetaEngine::Audio::GetInstance()->LoadWave("Enemythurst.wav");*/

    // 振る舞い初期化
    ChangeDamageReactionBehavior(std::make_unique<EnemyDamageReactionRoot>(this));
    ChangeBehavior(std::make_unique<EnemySpawn>(this));
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

    collisionBox_->SetPosition(GetWorldPosition());
    collisionBox_->Update();

    // エフェクト更新
    if (enemyEffects_) {
        enemyEffects_->Update(GetWorldPosition());
    }

    MoveToLimit();

    BaseObject::Update();
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

    Vector2 findPos(positionScreen.x, positionScreen.y - 100.0f);

    // HPBarスプライト
    findSprite_->SetPosition(findPos);
    // Hpバー更新
    findSprite_->Update();

    // HPBarスプライト
    notFindSprite_->SetPosition(findPos);
    // Hpバー更新
    notFindSprite_->Update();

    findSprite_->SetIsDraw(IsInView(viewProjection));
    notFindSprite_->SetIsDraw(IsInView(viewProjection));
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

    if (SideRope* sideRope = dynamic_cast<SideRope*>(other)) {

        if (EnemyDamageReactionAction* action = dynamic_cast<EnemyDamageReactionAction*>(damageBehavior_.get())) {
            Vector3 velocity = action->GetKnockBackVelocity();
            ChangeDamageReactionBehavior(std::make_unique<EnemyRopeBoundReaction>(this, velocity, sideRope));
            return;
        }

    } else if (PlayerCollisionInfo* attackController = dynamic_cast<PlayerCollisionInfo*>(other)) {
        // プレイヤーとの攻撃コリジョン判定
        ChangeDamageReactionByPlayerAttack(attackController);
        return;
    }

     if (BaseEnemy* enemy = dynamic_cast<BaseEnemy*>(other)) {
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
            /// それぞれ片方ずるめり込んでいる
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

void BaseEnemy::ChangeDamageReactionByPlayerAttack(PlayerCollisionInfo* attackController) {

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

Vector3 BaseEnemy::GetCollisionPos() const {
    // ローカル座標でのオフセット
    const Vector3 offset = Vector3::ZeroVector();
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

void BaseEnemy::DamageRenditionInit() {
  
}

void BaseEnemy::ThrustRenditionInit() {
    // ガレキパーティクル
    pEnemyManager_->ThrustEmit(GetWorldPosition());
    /*  KetaEngine::Audio::GetInstance()->PlayWave(thrustSound_, 0.2f);*/
}

void BaseEnemy::DeathRenditionInit() {
    pEnemyManager_->DeathEmit(GetWorldPosition());
    /* KetaEngine::Audio::GetInstance()->PlayWave(deathSound_, 0.5f);*/
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

void BaseEnemy::SetGameCamera(GameCamera* gameCamera) {
    pGameCamera_ = gameCamera;
}

void BaseEnemy::SetManager(EnemyManager* manager) {
    pEnemyManager_ = manager;
}

void BaseEnemy::SetCombo(Combo* manager) {
    pCombo_ = manager;
}
void BaseEnemy::SetAttackEffect(AttackEffect* attackEffect) {
    pAttackEffect_ = attackEffect;
}

void BaseEnemy::BackToDamageRoot() {
    ChangeDamageReactionBehavior(std::make_unique<EnemyDamageReactionRoot>(this));
}

void BaseEnemy::SetParameter(const Type& type, const Parameter& parameter) {
    type_      = type;
    parameter_ = parameter;
}
void BaseEnemy::SetBodyColor(const Vector4& color) {
    obj3d_->objColor_.SetColor(color);
}

void BaseEnemy::RotateInit() {
    obj3d_->transform_.rotation_ = Vector3::ZeroVector();
}

void BaseEnemy::ScaleReset() {
    baseTransform_.scale_ = parameter_.initScale_;
}