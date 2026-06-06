#pragma once

#include "3d/Object3D/Object3d.h"
#include "BaseEnemy.h"
#include "Easing/Easing.h"
#include "Easing/EasingCreator/EasingCreator.h"
#include "Easing/EasingCreator/EasingParameterData.h"
#include "Enemy/AttackBomb/BossAttackBombManager.h"
#include <functional>
#include <memory>

/// <summary>
/// 強敵クラス
/// </summary>
class StrongEnemy : public BaseEnemy {
public:
    struct StrongParameter {
        float fleeSpeed         = 5.0f;
        float fleeDistance      = 20.0f;
        float fleeCooldownTime  = 0.5f;
        float separationDistance;
        float separationStrength;
        Vector3 tauntFontOffset;
        float tauntFontRotateSpeed;
    };

public:
    StrongEnemy() = default;
    ~StrongEnemy() override;

    void Init(const Vector3& spownPos) override;
    void Update() override;
    void SpawnRenditionInit() override;

    void StartFlee();
    void StartTaunt();
    void StopTaunt();
    void StopTauntToWait(float waitTime);
    void BackToDamageRoot() override;
    std::unique_ptr<BaseEnemyBehavior> CreatePostSpawnBehavior() override;

private:
    bool isTaunting_        = false;
    bool isTauntFontMoving_ = false;
    bool isFleeing_         = false;
    StrongParameter strongParam_;

    // Taunt演出用フォントオブジェクト
    std::unique_ptr<KetaEngine::Object3d> tauntFont_;

    // 攻撃ボム管理
    std::unique_ptr<BossAttackBombManager> bombManager_;

    KetaEngine::Easing<Vector3> colorEasing_;
    Vector3 currentFontColor_ = {1.0f, 1.0f, 1.0f};

    void PlayTauntFontSpawn();
    void PlayTauntFontMoving();
    void PlayTauntFontClose();
    void CreateAndSetupTauntBehavior();

    static Vector3 RandomBrightColor();
    void StartNextColorTransition();

public:
    const StrongParameter& GetStrongParameter() const { return strongParam_; }
    void SetStrongParameter(const StrongParameter& param) { strongParam_ = param; }
    bool IsTaunting() const { return isTaunting_; }

    KetaEngine::Object3d* GetTauntFont() const { return tauntFont_.get(); }
};
