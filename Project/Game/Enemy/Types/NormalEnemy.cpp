#include "NormalEnemy.h"
#include "audio/Audio.h"
#include "Enemy/EnemyManager.h"
#include "Frame/Frame.h"

///========================================================
///  初期化
///========================================================
void NormalEnemy::Init(const Vector3& spownPos) {
    BaseEnemy::Init(spownPos);

    // オブジェ生成
    objAnimation_.reset(KetaEngine::Object3DAnimation::CreateModel("BoxerEnemyWait.gltf"));
    // 初期化
    objAnimation_->Init();
    // アニメーション追加
    objAnimation_->Add("BoxerEnemyPreDashMotion.gltf");
    objAnimation_->Add("BoxerEnemyDash.gltf");
    // transform初期化設定
    objAnimation_->transform_.Init();
    objAnimation_->transform_.SetParent(&baseTransform_);
    objAnimation_->transform_.scale_ = Vector3::OneVector();

    // ライティング番号
    objAnimation_->GetModelMaterial()->GetMaterialData()->enableLighting = 3;

    // 攻撃ステート初期化
    attackAnticipationTimer_      = 0.0f;
    isAttackAnticipationFinished_ = false;
    isAttackFinished_             = false;

    // 追跡アニメーション状態初期化
    chaseAnimState_    = ChaseAnimationState::NONE;
    isPreDashFinished_ = false;
}

///========================================================
/// 更新
///========================================================
void NormalEnemy::Update() {
    BaseEnemy::Update();
}

void NormalEnemy::SpawnRenditionInit() {
    GetEnemyEffects()->Emit("SpawnEffect");
}

///========================================================
/// 攻撃予備動作
///========================================================
void NormalEnemy::AttackAnticipation() {
    // タイマー更新
    attackAnticipationTimer_ += KetaEngine::Frame::DeltaTimeRate();

    // 予備動作時間が経過したら完了フラグを立てる
    if (attackAnticipationTimer_ >= parameter_.attackAnticipationTime) {
        isAttackAnticipationFinished_ = true;
    }
}

///========================================================
/// 攻撃予備動作完了判定
///========================================================
bool NormalEnemy::IsAttackAnticipationFinished() {
    return isAttackAnticipationFinished_;
}

///========================================================
/// 攻撃開始処理
///========================================================
void NormalEnemy::AttackStart() {
    // 攻撃フラグリセット
    isAttackFinished_ = false;

    // スケールを元に戻す
    SetScale(parameter_.baseScale_);

    // 攻撃コリジョンを設定して開始
    const auto& attackParam = parameter_.attackParam;
    attackCollisionBox_->AttackStart(
        attackParam.attackValue,
        attackParam.collisionSize,
        attackParam.collisionOffset,
        attackParam.adaptTime);

    // 攻撃エフェクト再生
    if (enemyEffects_) {
        enemyEffects_->Emit("AttackEffect");
    }

    // 攻撃音再生
    // KetaEngine::Audio::GetInstance()->PlaySE("EnemyAttack.wav", 0.5f);
}

///========================================================
/// 攻撃更新処理
///========================================================
void NormalEnemy::AttackUpdate() {
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
bool NormalEnemy::IsAttackFinished() {
    return isAttackFinished_;
}

///========================================================
/// 攻撃終了処理
///========================================================
void NormalEnemy::AttackFinish() {
    // 攻撃ステートをリセット
    attackAnticipationTimer_      = 0.0f;
    isAttackAnticipationFinished_ = false;
    isAttackFinished_             = false;

    // 攻撃から戻ってきたフラグを立てる
    SetIsReturningFromAttack(true);
}

///========================================================
/// HpBar表示
///========================================================
void NormalEnemy::DisplaySprite(const KetaEngine::ViewProjection& viewProjection) {
    BaseEnemy::DisplaySprite(viewProjection);
}

///========================================================
/// 追跡開始時のアニメーション初期化
///========================================================
void NormalEnemy::InitChaseAnimation() {
    if (!objAnimation_ && chaseAnimState_ == ChaseAnimationState::NONE) {
        return;
    }

    // ダッシュ予備動作アニメーションに切り替え
    chaseAnimState_    = ChaseAnimationState::PRE_DASH;
    isPreDashFinished_ = false;
    objAnimation_->ChangeAnimation("BoxerEnemyPreDashMotion");
    objAnimation_->SetLoop(false); // 予備動作はループしない

    // アニメーション終了時のコールバックを設定
    objAnimation_->SetAnimationEndCallback([this](const std::string& animationName) {
        if (animationName == "BoxerEnemyPreDashMotion") {
            isPreDashFinished_ = true;
        }
    });
}

///========================================================
/// 追跡中のアニメーション更新
///========================================================
void NormalEnemy::UpdateChaseAnimation(float deltaTime) {
    if (!objAnimation_) {
        return;
    }
    deltaTime;
    // 予備動作が終了したらダッシュアニメーションに切り替え
    if (chaseAnimState_ == ChaseAnimationState::PRE_DASH && isPreDashFinished_) {
        chaseAnimState_ = ChaseAnimationState::DASHING;
        objAnimation_->ChangeAnimation("BoxerEnemyDash");
        objAnimation_->SetLoop(true); // ダッシュはループ
    }

}

///========================================================
/// 待機アニメーションにリセット
///========================================================
void NormalEnemy::ResetToWaitAnimation() {
    if (!objAnimation_) {
        return;
    }

    // 待機アニメーションに戻す
    chaseAnimState_    = ChaseAnimationState::NONE;
    isPreDashFinished_ = false;
    objAnimation_->ChangeAnimation("BoxerEnemyWait.gltf");
    objAnimation_->SetLoop(true); // 待機アニメーションはループ
}
