#include "NormalEnemy.h"
#include "../Behavior/AttackStrategy/NormalEnemyAttackStrategy.h"
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

    // 攻撃戦略を設定
    SetAttackStrategy(std::make_unique<NormalEnemyAttackStrategy>(this));

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
