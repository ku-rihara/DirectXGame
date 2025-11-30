#include "BaseEnemy.h"

// behavior
#include "../Behavior/DamageReactionBehavior/EnemyDamageReactionRoot.h"
#include "../Behavior/NormalBehavior/EnemySpawn.h"
#include "Enemy/EnemyManager.h"

#include "Combo/Combo.h"

/// collisionBox
#include "CollisionBox/PlayerCollisionInfo.h"
#include"Player/ComboCreator/PlayerComboAttackData.h"

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
    DamageCollingUpdate(Frame::DeltaTimeRate());

    // ダメージBehavior更新
    damageBehavior_->Update();

    // 死んだら死亡Behaviorに切り替え
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

    //
    if (PlayerCollisionInfo* attackController = dynamic_cast<PlayerCollisionInfo*>(other)) {

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

void BaseEnemy::BehaviorChangeDeath() {
    if (hp_ > 0) {
        return;
    }
    /* if (dynamic_cast<EnemyDeath*>(damageBehavior_.get()) || dynamic_cast<EnemyThrustDamage*>(damageBehavior_.get())) {
         return;
     };

     isAdaptCollision = false;
     collisionBox_->SetIsCollision(false);
     ChangeDamageReactionBehavior(std::make_unique<EnemyDeath>(this));*/
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

void BaseEnemy::StartDamageColling(const float& collingTime, const std::string& reactiveAttackName) {
    isDamageColling_        = true;
    lastReceivedAttackName_ = reactiveAttackName;
    damageCollTime_         = collingTime;
}

void BaseEnemy::DamageCollingUpdate(const float& deltaTime) {
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
    obj3d_->transform_.rotation_ = {0.0f, 0.0f, 0.0f};
}

void BaseEnemy::ScaleReset() {
    baseTransform_.scale_ = parameter_.initScale_;
}
