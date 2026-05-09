#include "StrongEnemy.h"
#include "Editor/ObjEaseAnimation/ObjEaseAnimationPlayer.h"
// behavior
#include "Enemy/Behavior/ActionBehavior/CommonBehavior/EnemyChase.h"
#include "Enemy/Behavior/ActionBehavior/CommonBehavior/EnemySpawn.h"
#include "Enemy/Behavior/ActionBehavior/CommonBehavior/EnemyWait.h"
#include "Enemy/Behavior/ActionBehavior/StrongEnemyBehavior/StrongEnemyFleeBehavior.h"
#include "Enemy/Behavior/ActionBehavior/StrongEnemyBehavior/StrongEnemyTauntBehavior.h"
#include "Enemy/Behavior/DamageReactionBehavior/EnemyDamageReactionRoot.h"
// manager
#include "Enemy/EnemyManager.h"

#include "Frame/Frame.h"
#include "Light/LightingType.h"
#include "math/MathFunction.h"
#include "math/random.h"
#include <cmath>
#include <imgui.h>

StrongEnemy::~StrongEnemy() {
    SetOnDamageTakenCallback(nullptr);
}

void StrongEnemy::Init(const Vector3& spawnPos) {
    BaseEnemy::Init(spawnPos);

    // アニメーション名の設定
    SetAnimationName(AnimationType::Wait, "StrongEnemyWaiting");
    SetAnimationName(AnimationType::Spawn, "StrongEnemySpawn");
    SetAnimationName(AnimationType::Discovery, "StrongEnemyDiscovery");
    SetAnimationName(AnimationType::Dash, "StrongEnemyRun");
    SetAnimationName(AnimationType::Death, "StrongEnemyDeath");
    SetAnimationName(AnimationType::Taunt, "StrongEnemyTaunt");

    // ダメージリアクションのアニメーションを追加
    AddDamageReactionAnimation("StrongEnemyDefaultDamage");
    AddDamageReactionAnimation("StrongEnemyTakeUp", true);
    AddDamageReactionAnimation("StrongEnemyBoundDamage");
    AddDamageReactionAnimation("StrongEnemyKipUp");

    // アニメーションの初期化
    objAnimation_->transform_.Init();
    objAnimation_->transform_.SetParent(&baseTransform_);
    objAnimation_->transform_.scale_                                     = Vector3::OneVector();
    objAnimation_->GetModelMaterial()->GetMaterialData()->enableLighting = static_cast<int32_t>(KetaEngine::LightingType::SpecularReflection);
 

    // Tauntフォントオブジェクト初期化
    tauntFont_.reset(KetaEngine::Object3d::CreateModel("Font/IrairaDance.obj"));
    tauntFont_->transform_.Init();
    tauntFont_->transform_.translation_ = strongParam_.tauntFontOffset;
    tauntFont_->transform_.scale_       = Vector3::ZeroVector();

    // 色イージング初期化
    colorEasing_.Init("TauntColor");

    // スポーン後の行動を生成
    BaseEnemy::ChangeBehavior(std::make_unique<EnemySpawn>(this));
}

void StrongEnemy::Update() {
    BaseEnemy::Update();

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
}

void StrongEnemy::SpawnRenditionInit() {
    GetEnemyEffects()->Emit("SpawnEffectStrong");
}

// ランダムな明るい色をHSVから生成
Vector3 StrongEnemy::RandomBrightColor() {
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
void StrongEnemy::StartNextColorTransition() {

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
void StrongEnemy::PlayTauntFontSpawn() {
    isTauntFontMoving_ = false;
    if (tauntFont_) {
        tauntFont_->transform_.scale_ = Vector3::OneVector();
        tauntFont_->transform_.PlayObjEaseAnimation("SpawnTauntFont", "StrongEnemyTauntFont");
    }
}

// フォントをループアニメーションへ移行 
void StrongEnemy::PlayTauntFontMoving() {
    isTauntFontMoving_ = true;
    if (tauntFont_) {
        tauntFont_->transform_.PlayObjEaseAnimation("TauntFontMoving", "StrongEnemyTauntFont");
    }
    StartNextColorTransition();
}

// フォントを閉じるアニメーションへ移行
void StrongEnemy::PlayTauntFontClose() {
    isTauntFontMoving_ = false;
    if (tauntFont_) {
        tauntFont_->transform_.PlayObjEaseAnimation("CloseTauntFont", "StrongEnemyTauntFont");
        auto* player = tauntFont_->transform_.GetObjEaseAnimationPlayer();
        if (player) {
            player->SetEndCallback([this]() {
                tauntFont_->transform_.scale_ = Vector3::ZeroVector();
            });
        }
    }
}

// ビヘイビア生成
void StrongEnemy::CreateAndSetupTauntBehavior() {
    auto behavior = std::make_unique<StrongEnemyTauntBehavior>(this);

    // ビヘイビアのコールバックを設定
    behavior->SetOnFontMovingStart([this]() { PlayTauntFontMoving(); });

    // Behavior
    ChangeBehavior(std::move(behavior));
}

std::unique_ptr<BaseEnemyBehavior> StrongEnemy::CreatePostSpawnBehavior() {
    return std::make_unique<StrongEnemyFleeBehavior>(this);
}

void StrongEnemy::StartFlee() {
    isTaunting_ = false;
    PlayTauntFontClose();
    SetOnDamageTakenCallback(nullptr);
    ChangeBehavior(std::make_unique<StrongEnemyFleeBehavior>(this, strongParam_.fleeCooldownTime));
}

void StrongEnemy::StartTaunt() {
    if (isTaunting_) {
        return;
    }
    if (dynamic_cast<EnemySpawn*>(moveBehavior_.get())) {
        return;
    }

    isTaunting_ = true;
    PlayTauntFontSpawn();
    GetEnemyEffects()->Emit("TauntBoss");
    CreateAndSetupTauntBehavior();

    // ダメージを受けた瞬間にTaunt演出を終了させる
    SetOnDamageTakenCallback([this]() {
        PlayTauntFontClose();
    });
}

void StrongEnemy::StopTaunt() {
    if (!isTaunting_) {
        return;
    }

    isTaunting_ = false;
    // 追いかけBehaviorに遷移
    ChangeBehavior(std::make_unique<EnemyChase>(this));
}

void StrongEnemy::StopTauntToWait(float waitTime) {
    if (!isTaunting_) {
        return;
    }

    isTaunting_ = false;
    // 待機Behaviorに遷移
    ChangeBehavior(std::make_unique<EnemyWait>(this, waitTime));
}

void StrongEnemy::BackToDamageRoot() {

    // ダメージリアクションRootだけ設定して即座に逃走へ切り替える
    ChangeDamageReactionBehavior(std::make_unique<EnemyDamageReactionRoot>(this));
    StartFlee();
}


