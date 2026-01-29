#include "StrongEnemy.h"
#include "audio/Audio.h"
#include "Enemy/EnemyManager.h"
#include "Frame/Frame.h"

///========================================================
///  初期化
///========================================================
void StrongEnemy::Init(const Vector3& spawnPos) {
    BaseEnemy::Init(spawnPos);
    obj3d_.reset(KetaEngine::Object3d::CreateModel("StrongEnemy.obj"));
    obj3d_->transform_.Init();
    obj3d_->transform_.SetParent(&baseTransform_);
    obj3d_->GetModelMaterial()->GetMaterialData()->enableLighting = 2;

    // 攻撃ステート初期化
    attackAnticipationTimer_ = 0.0f;
    isAttackAnticipationFinished_ = false;
    isAttackFinished_ = false;
}

///========================================================
/// 更新
///========================================================
void StrongEnemy::Update() {
    BaseEnemy::Update();
}

void StrongEnemy::SpawnRenditionInit() {
    GetEnemyEffects()->Emit("SpawnEffectStrong");
}

///========================================================
/// 攻撃予備動作
///========================================================
void StrongEnemy::AttackAnticipation() {
    // タイマー更新
    attackAnticipationTimer_ += KetaEngine::Frame::DeltaTimeRate();

    // アニメーション再生

    // 予備動作時間が経過したら完了フラグを立てる
    if (attackAnticipationTimer_ >= parameter_.attackAnticipationTime) {
        isAttackAnticipationFinished_ = true;
    }
}

///========================================================
/// 攻撃予備動作完了判定
///========================================================
bool StrongEnemy::IsAttackAnticipationFinished() {
    return isAttackAnticipationFinished_;
}

///========================================================
/// 攻撃開始処理
///========================================================
void StrongEnemy::AttackStart() {
    // 攻撃フラグリセット
    isAttackFinished_ = false;

    // スケールと回転を元に戻す
    SetScale(parameter_.baseScale_);
    SetBodyRotateX(0.0f);

    // 攻撃コリジョンを設定して開始
    const auto& attackParam = parameter_.attackParam;
    attackCollisionBox_->AttackStart(
        attackParam.attackValue,
        attackParam.collisionSize,
        attackParam.collisionOffset,
        attackParam.adaptTime
    );

    // 攻撃エフェクト再生（強敵は派手なエフェクト）
    if (enemyEffects_) {
        enemyEffects_->Emit("AttackEffectStrong");
    }

    // 攻撃音再生
    // KetaEngine::Audio::GetInstance()->PlaySE("StrongEnemyAttack.wav", 0.7f);
}

///========================================================
/// 攻撃更新処理
///========================================================
void StrongEnemy::AttackUpdate() {
    // コリジョンが終了したか確認
    if (attackCollisionBox_->GetIsFinish()) {
        isAttackFinished_ = true;
    }

    // プレイヤーにヒットした場合の処理
    if (attackCollisionBox_->GetIsHit()) {
      
        // ヒットフラグをリセット
        attackCollisionBox_->SetIsHit(false);
    }
}

///========================================================
/// 攻撃完了判定
///========================================================
bool StrongEnemy::IsAttackFinished() {
    return isAttackFinished_;
}

///========================================================
/// 攻撃終了処理
///========================================================
void StrongEnemy::AttackFinish() {
    // 攻撃ステートをリセット
    attackAnticipationTimer_ = 0.0f;
    isAttackAnticipationFinished_ = false;
    isAttackFinished_ = false;

    // 攻撃から戻ってきたフラグを立てる
    SetIsReturningFromAttack(true);
}

///========================================================
/// HpBar表示
///========================================================
void StrongEnemy::DisplaySprite(const KetaEngine::ViewProjection& viewProjection) {
    BaseEnemy::DisplaySprite(viewProjection);
}

///========================================================
/// 追跡開始時のアニメーション初期化（StrongEnemy用）
///========================================================
void StrongEnemy::InitChaseAnimation() {
    
}

///========================================================
/// 追跡中のアニメーション更新（StrongEnemy用）
///========================================================
void StrongEnemy::UpdateChaseAnimation(float deltaTime) {
    deltaTime;
}

///========================================================
/// 待機アニメーションにリセット（StrongEnemy用）
///========================================================
void StrongEnemy::ResetToWaitAnimation() {
   
}
