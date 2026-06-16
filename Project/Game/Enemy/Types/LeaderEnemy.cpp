#include "LeaderEnemy.h"
#include "StressGauge/StressGauge.h"
#include "Editor/ObjEaseAnimation/ObjEaseAnimationPlayer.h"
#include "Player/Player.h"
// behavior
#include "Enemy/Behavior/ActionBehavior/CommonBehavior/EnemyChase.h"
#include "Enemy/Behavior/ActionBehavior/CommonBehavior/EnemySpawn.h"
#include "Enemy/Behavior/ActionBehavior/CommonBehavior/EnemyWait.h"
#include "Enemy/Behavior/ActionBehavior/LeaderEnemyBehavior/LeaderEnemyFleeBehavior.h"
#include "Enemy/Behavior/ActionBehavior/LeaderEnemyBehavior/LeaderEnemyTauntBehavior.h"
#include "Enemy/Behavior/DamageReactionBehavior/EnemyDamageReactionRoot.h"
// manager
#include "Enemy/EnemyManager/EnemyManager.h"

#include "Frame/Frame.h"
#include "Light/LightingType.h"
#include "math/MathFunction.h"
#include "math/random.h"
#include <cmath>


LeaderEnemy::~LeaderEnemy() {
    SetOnDamageTakenCallback(nullptr);
    if (bombManager_) {
        bombManager_->Stop();
    }
}

void LeaderEnemy::PrepareForPool() {
    if (bombManager_) {
        bombManager_->Stop();
    }
    BaseEnemy::PrepareForPool();
}

void LeaderEnemy::Init(const Vector3& spawnPos) {
    isTaunting_        = false;
    isTauntFontMoving_ = false;
    isFleeing_         = false;

    // 基底クラスの初期化
    BaseEnemy::Init(spawnPos);
    // アニメーション初期化
    InitAnimations();
    // 煽り文字の初期化
    InitTauntFont();
    // 攻撃ボムマネージャーの初期化
    InitBombManager();

    // Behaviorのセットアップ
    BaseEnemy::ChangeBehavior(std::make_unique<EnemySpawn>(this));
}

void LeaderEnemy::InitAnimations() {
    GetAnimator()->SetAnimationName(AnimationType::Wait, "LeaderEnemyWaiting");
    GetAnimator()->SetAnimationName(AnimationType::Spawn, "LeaderEnemySpawn");
    GetAnimator()->SetAnimationName(AnimationType::Discovery, "LeaderEnemyDiscovery");
    GetAnimator()->SetAnimationName(AnimationType::Dash, "LeaderEnemyRun");
    GetAnimator()->SetAnimationName(AnimationType::Death, "LeaderEnemyDeath");
    GetAnimator()->SetAnimationName(AnimationType::Taunt, "LeaderEnemyTaunt");

    GetAnimator()->AddDamageReactionAnimation("LeaderEnemyDefaultDamage");
    GetAnimator()->AddDamageReactionAnimation("LeaderEnemyTakeUp", true);
    GetAnimator()->AddDamageReactionAnimation("LeaderEnemyBoundDamage");
    GetAnimator()->AddDamageReactionAnimation("LeaderEnemyKipUp");

    auto* animObj = GetAnimator()->GetAnimationObject();
    animObj->transform_.Init();
    animObj->transform_.SetParent(&baseTransform_);
    animObj->transform_.scale_                                     = Vector3::OneVector();
    animObj->GetModelMaterial()->GetMaterialData()->enableLighting = static_cast<int32_t>(KetaEngine::LightingType::SpecularReflection);
}

void LeaderEnemy::InitTauntFont() {
    if (!tauntFont_) {
        tauntFont_.reset(KetaEngine::Object3d::CreateModel("Font/IrairaDance.obj"));
    }
    // Transformの初期化
    tauntFont_->transform_.Init();
    tauntFont_->transform_.translation_ = strongParam_.tauntFontOffset;
    tauntFont_->transform_.scale_       = Vector3::ZeroVector();

    // マテリアルの初期化
    colorEasing_.Init("TauntColor");
}

void LeaderEnemy::InitBombManager() {

    // 攻撃ボムマネージャーの生成
    if (!bombManager_) {
        bombManager_ = std::make_unique<BossAttackBombManager>();
    }

    // 攻撃ボムマネージャーの初期化
    bombManager_->Init();

    // カメラとストレスタイマーのセット
    bombManager_->SetGameCamera(GetBaseInfo()->GetGameCamera());
    if (auto* player = GetBaseInfo()->GetPlayer()) {
        bombManager_->SetStressGauge(player->GetContext().pStressGauge);
        bombManager_->SetPlayer(player);
    }
    bombManager_->SetParam(GetBaseInfo()->GetManager()->GetBombParam());
}

void LeaderEnemy::Update() {

    // エフェクトのターゲット座標をセットする
    if (GetBaseInfo()->GetPlayer()) {
        Vector3 playerPos = GetBaseInfo()->GetPlayer()->GetWorldPosition();
        GetEnemyEffects()->SetTargetObjectPos({playerPos.x, 0.0f, playerPos.z});
    }

    // 基底クラスの更新
    BaseEnemy::Update();

    // 煽り文字の更新
    if (bombManager_) {
        bombManager_->SetBossPosition(baseTransform_.translation_);
        if (auto* player = GetBaseInfo()->GetPlayer()) {
            bombManager_->SetPlayerPosition(player->GetWorldPosition());
        }
        bombManager_->Update();
    }

    // フォント位置を毎フレームbaseTransformに追従
    if (tauntFont_) {
        tauntFont_->transform_.translation_ = baseTransform_.translation_ + strongParam_.tauntFontOffset;
    }

    if (isTauntFontMoving_) {
        // 色イージング更新・適用
        colorEasing_.Update(KetaEngine::Frame::DeltaTimeRate());
        tauntFont_->GetModelMaterial()->GetMaterialData()->color =
            Vector4{currentFontColor_.x, currentFontColor_.y, currentFontColor_.z, 1.0f};

        // Y軸回転
        tauntFont_->transform_.rotation_.y += strongParam_.tauntFontRotateSpeed * KetaEngine::Frame::DeltaTimeRate();

        // 毎フレームEmit
        GetEnemyEffects()->Emit("TauntBoss");
    }

    if (isFleeing_ && dynamic_cast<EnemyDamageReactionRoot*>(GetDamageReactionBehavior())) {
        // 焦りエフェクトの発生
        GetEnemyEffects()->Emit("EnemyImpatience");
    }
}

void LeaderEnemy::SpawnRenditionInit() {
    GetEnemyEffects()->Emit("SpawnEffectStrong");
}

// ランダムな明るい色をHSVから生成
Vector3 LeaderEnemy::RandomBrightColor() {
    float hue      = Random::Range(0.0f, 1.0f);
    float sector   = std::floor(hue * 6.0f);
    float fraction = hue * 6.0f - sector;
    float fadeOut  = 1.0f - fraction;
    float r, g, b;
    switch (static_cast<int>(sector) % 6) {
    case 0:
        r = 1.0f;
        g = fraction;
        b = 0.0f;
        break;
    case 1:
        r = fadeOut;
        g = 1.0f;
        b = 0.0f;
        break;
    case 2:
        r = 0.0f;
        g = 1.0f;
        b = fraction;
        break;
    case 3:
        r = 0.0f;
        g = fadeOut;
        b = 1.0f;
        break;
    case 4:
        r = fraction;
        g = 0.0f;
        b = 1.0f;
        break;
    default:
        r = 1.0f;
        g = 0.0f;
        b = fadeOut;
        break;
    }
    return {r, g, b};
}

// 次の色遷移を開始
void LeaderEnemy::StartNextColorTransition() {

    // 始点、終点を設定
    colorEasing_.SetStartValue(currentFontColor_);
    colorEasing_.SetEndValue(RandomBrightColor());

    colorEasing_.SetAdaptValue(&currentFontColor_);

    // 終了コールバック
    colorEasing_.SetOnFinishCallback([this]() {
        if (isTauntFontMoving_) {
            StartNextColorTransition();
        }
    });
    colorEasing_.Reset();
}

// フォントをスポーンアニメーションで表示
void LeaderEnemy::PlayTauntFontSpawn() {
    isTauntFontMoving_ = false;
    if (tauntFont_) {
        tauntFont_->transform_.scale_ = Vector3::OneVector();
        tauntFont_->transform_.PlayObjEaseAnimation("SpawnTauntFont", "LeaderEnemyTauntFont");
    }
}

// フォントをループアニメーションへ移行
void LeaderEnemy::PlayTauntFontMoving() {
    isTauntFontMoving_ = true;
    if (tauntFont_) {
        tauntFont_->transform_.PlayObjEaseAnimation("TauntFontMoving", "LeaderEnemyTauntFont");
    }
    StartNextColorTransition();
}

// フォントを閉じるアニメーションへ移行
void LeaderEnemy::PlayTauntFontClose() {
    isTauntFontMoving_ = false;
    if (tauntFont_) {
        tauntFont_->transform_.PlayObjEaseAnimation("CloseTauntFont", "LeaderEnemyTauntFont");
        auto* player = tauntFont_->transform_.GetObjEaseAnimationPlayer();
        if (player) {
            player->SetEndCallback([this]() {
                tauntFont_->transform_.scale_ = Vector3::ZeroVector();
            });
        }
    }
}

// ビヘイビア生成
void LeaderEnemy::CreateAndSetupTauntBehavior() {
    auto behavior = std::make_unique<LeaderEnemyTauntBehavior>(this);

    // ビヘイビアのコールバックを設定
    behavior->SetOnFontMovingStart([this]() { PlayTauntFontMoving(); });

    // Behavior
    ChangeBehavior(std::move(behavior));
}

std::unique_ptr<BaseEnemyBehavior> LeaderEnemy::CreatePostSpawnBehavior() {
    return std::make_unique<LeaderEnemyFleeBehavior>(this);
}

void LeaderEnemy::StartFlee() {
    isTaunting_ = false;
    isFleeing_  = true;
    if (bombManager_) {
        bombManager_->Stop();
    }
    PlayTauntFontClose();
    SetOnDamageTakenCallback(nullptr);
    GetEnemyEffects()->Emit("EnemyImpatience");
    ChangeBehavior(std::make_unique<LeaderEnemyFleeBehavior>(this, strongParam_.fleeCooldownTime));
}

void LeaderEnemy::StartTaunt() {
    if (isTaunting_) {
        return;
    }
    if (dynamic_cast<EnemySpawn*>(behaviorCtrl_.GetMoveBehavior())) {
        return;
    }

    isTaunting_ = true;
    isFleeing_  = false;
    PlayTauntFontSpawn();
    GetEnemyEffects()->Emit("TauntBoss");
    CreateAndSetupTauntBehavior();
    if (bombManager_) {
        bombManager_->Start();
    }

    // ダメージを受けた瞬間にTaunt演出を終了させる
    SetOnDamageTakenCallback([this]() {
        PlayTauntFontClose();
    });
}

void LeaderEnemy::StopTaunt() {
    if (!isTaunting_) {
        return;
    }

    isTaunting_ = false;
    isFleeing_  = false;
    if (bombManager_) {
        bombManager_->Stop();
    }
    // 追いかけBehaviorに遷移
    ChangeBehavior(std::make_unique<EnemyChase>(this));
}

void LeaderEnemy::StopTauntToWait(float waitTime) {
    if (!isTaunting_) {
        return;
    }

    isTaunting_ = false;
    isFleeing_  = false;
    // 待機Behaviorに遷移
    ChangeBehavior(std::make_unique<EnemyWait>(this, waitTime));
}

void LeaderEnemy::BackToDamageRoot() {
    isFleeing_ = false;
    // ダメージリアクションRootだけ設定して即座に逃走へ切り替える
    ChangeDamageReactionBehavior(std::make_unique<EnemyDamageReactionRoot>(this));
    StartFlee();
}
