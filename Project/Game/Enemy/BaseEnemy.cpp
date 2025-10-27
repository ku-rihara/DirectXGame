#include "BaseEnemy.h"

// behavior
#include "Behavior/EnemyBoundDamage.h"
#include "Behavior/EnemyDamageRoot.h"
#include "Behavior/EnemyDeath.h"
#include "Behavior/EnemyHitBackDamage.h"
#include "Behavior/EnemySpawn.h"
#include "Behavior/EnemyStopDamage.h"
#include "Behavior/EnemyThrustDamage.h"
#include "Behavior/EnemyUpperDamage.h"
#include "Enemy/EnemyManager.h"

#include "Combo/Combo.h"

/// collisionBox
#include "CollisionBox/PlayerAttackController.h"

#include "AttackEffect/AttackEffect.h"
#include "audio/Audio.h"
#include "Frame/Frame.h"
#include "GameCamera/GameCamera.h"
#include "Matrix4x4.h"
#include "Player/Player.h"

///========================================================
///  初期化
///========================================================
void BaseEnemy::Init(const Vector3& spawnPos) {

    // HP
    HPMax_ = 105.0f;
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

    // audio
    deathSound_  = Audio::GetInstance()->LoadWave("EnemyDeath.wav");
    thrustSound_ = Audio::GetInstance()->LoadWave("Enemythurst.wav");

    // 振る舞い初期化
    ChangeBehavior(std::make_unique<EnemyDamageRoot>(this));
    ChangeMoveBehavior(std::make_unique<EnemySpawn>(this));
}

///========================================================
/// 更新
///========================================================
void BaseEnemy::Update() {

    if (dynamic_cast<EnemyDamageRoot*>(damageBehavior_.get())) {
        moveBehavior_->Update();
    }

    damageBehavior_->Update();

    BehaviorChangeDeath();

    collisionBox_->SetPosition(GetWorldPosition());
    collisionBox_->Update();
    BaseObject::Update();
}
///========================================================
/// HpBar表示
///========================================================
void BaseEnemy::DisplaySprite(const ViewProjection& viewProjection) {
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

    findSprite_->SetIsSpawned(IsInView(viewProjection));
    notFindSprite_->SetIsSpawned(IsInView(viewProjection));
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

    // 普通のパンチに攻撃されたら
    if (PlayerAttackController* attackController = dynamic_cast<PlayerAttackController*>(other)) {

        switch (attackController->attackType_) {
            ///------------------------------------------------------------------
            /// 通常
            ///------------------------------------------------------------------
        case PlayerAttackController::AttackType::NORMAL:

            TakeDamage(attackController->GetAttackPower());
            pCombo_->ComboCountUP();
            pGameCamera_->PlayShake("PunchAttackCamera");
            ChangeBehavior(std::make_unique<EnemyHitBackDamage>(this));
        }
    }
}

void BaseEnemy::OnCollisionStay([[maybe_unused]] BaseCollider* other) {

    if (PlayerAttackController* attackController = dynamic_cast<PlayerAttackController*>(other)) {

        // 攻撃タイプごとに振る舞いを切り替える
        switch (attackController->attackType_) {

            ///------------------------------------------------------------------
            /// アッパー
            ///------------------------------------------------------------------
        case PlayerAttackController::AttackType::UPPER:

            if (dynamic_cast<EnemyUpperDamage*>(damageBehavior_.get())) {
                break;
            }
            TakeDamage(attackController->GetAttackPower());
            pCombo_->ComboCountUP();
            ChangeBehavior(std::make_unique<EnemyUpperDamage>(this));

            break;

            ///------------------------------------------------------------------
            /// 突き飛ばし
            ///------------------------------------------------------------------
        case PlayerAttackController::AttackType::THRUST:
            if (dynamic_cast<EnemyThrustDamage*>(damageBehavior_.get())) {
                break;
            }
            TakeDamage(attackController->GetAttackPower());
            pCombo_->ComboCountUP();
            ChangeBehavior(std::make_unique<EnemyThrustDamage>(this));

            break;
            ///------------------------------------------------------------------
            /// 落下攻撃
            ///------------------------------------------------------------------
        case PlayerAttackController::AttackType::FALL:
            if (dynamic_cast<EnemyUpperDamage*>(damageBehavior_.get())) {
                break;
            }
            TakeDamage(attackController->GetAttackPower());
            pCombo_->ComboCountUP();
            ChangeBehavior(std::make_unique<EnemyUpperDamage>(this));

            return;

            break;
            ///------------------------------------------------------------------
            /// 突進
            ///------------------------------------------------------------------
        case PlayerAttackController::AttackType::RUSH:
            if (dynamic_cast<EnemyBoundDamage*>(damageBehavior_.get())) {
                break;
            }
            TakeDamage(attackController->GetAttackPower());
            pCombo_->ComboCountUP();
            ChangeBehavior(std::make_unique<EnemyUpperDamage>(this));

            break;
        default:
            break;
        }
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

void BaseEnemy::ChangeBehavior(std::unique_ptr<BaseEnemyBehavior> behavior) {
    // 引数で受け取った状態を次の状態としてセット
    damageBehavior_ = std::move(behavior);
}

void BaseEnemy::ChangeMoveBehavior(std::unique_ptr<BaseEnemyMoveBehavior> behavior) {
    moveBehavior_ = std::move(behavior);
}

void BaseEnemy::BehaviorChangeDeath() {
    if (hp_ > 0) {
        return;
    }
    if (dynamic_cast<EnemyDeath*>(damageBehavior_.get()) || dynamic_cast<EnemyThrustDamage*>(damageBehavior_.get())) {
        return;
    };

    isAdaptCollision = false;
    collisionBox_->SetIsCollision(false);
    ChangeBehavior(std::make_unique<EnemyDeath>(this));
}

bool BaseEnemy::IsInView(const ViewProjection& viewProjection) const {

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

void BaseEnemy::TakeDamage(const float& damageValue) {
    hp_ -= damageValue;

    if (hp_ < 0.0f) {
        hp_ = 0.0f;
    }
}

void BaseEnemy::DamageRenditionInit() {
    pEnemyManager_->DamageEffectShot(GetWorldPosition());
}

void BaseEnemy::ThrustRenditionInit() {
    // ガレキパーティクル
    pEnemyManager_->ThrustEmit(GetWorldPosition());
    Audio::GetInstance()->PlayWave(thrustSound_, 0.2f);
}

void BaseEnemy::DeathRenditionInit() {
    pEnemyManager_->DeathEmit(GetWorldPosition());
    Audio::GetInstance()->PlayWave(deathSound_, 0.5f);
}

/// ===================================================
///  BaseEnemy Jump
/// ===================================================
void BaseEnemy::Jump(float& speed, const float& fallSpeedLimit, const float& gravity) {
    // 移動
    baseTransform_.translation_.y += speed * Frame::DeltaTimeRate();
    Fall(speed, fallSpeedLimit, gravity, true);
}

///=========================================================
/// 　落ちる
///==========================================================
void BaseEnemy::Fall(float& speed, const float& fallSpeedLimit, const float& gravity, const bool& isJump) {

    if (!isJump) {
        // 移動
        baseTransform_.translation_.y += speed * Frame::DeltaTimeRate();
    }

    // 加速する
    speed = max(speed - (gravity * Frame::DeltaTimeRate()), -fallSpeedLimit);

    // 着地
    if (baseTransform_.translation_.y <= parameter_.basePosY) {
        baseTransform_.translation_.y = parameter_.basePosY;
        speed                         = 0.0f;
    }
}

void BaseEnemy::SetGameCamera(GameCamera* gamecamera) {
    pGameCamera_ = gamecamera;
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
    ChangeBehavior(std::make_unique<EnemyDamageRoot>(this)); /// 追っかけ
}

void BaseEnemy::SetParameter(const Type& type, const Parameter& paramater) {
    type_      = type;
    parameter_ = paramater;
}
void BaseEnemy::SetBodyColor(const Vector4& color) {
    obj3d_->objColor_.SetColor(color);
}

void BaseEnemy::RotateInit() {
    obj3d_->transform_.rotation_ = {0.0f, 0.0f, 0.0f};
}

void BaseEnemy::ScaleReset() {
    baseTransform_.scale_ = parameter_.initScale_;
}
