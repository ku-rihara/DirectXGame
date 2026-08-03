#pragma once

#include "3D/Object3D/Object3d.h"
#include "BaseEnemy.h"
#include "Editor/Easing/Easing.h"
#include "Enemy/AttackBomb/BossAttackBombManager.h"
#include <memory>

/// <summary>
/// 強敵クラス
/// </summary>
class LeaderEnemy : public BaseEnemy {
public:
    /// <summary>
    /// 強敵固有の逃走・離反・挑発表示に関するパラメータ
    /// </summary>
    struct StrongParameter {
        float fleeSpeed;
        float fleeDistance;
        float fleeCooldownTime;
        float separationDistance;
        float separationStrength;
        Vector3 tauntFontOffset;
        float tauntFontRotateSpeed;
        float fieldMargin; 
    };

public:
    LeaderEnemy() = default;
    ~LeaderEnemy() override;

    // 初期化、更新
    void Init(const Vector3& spownPos) override;
    void SpawnRenditionInit() override;
    void Update() override;

    // プール返却前クリーンアップ
    void PrepareForPool() override;

    // 逃走・煽りの状態遷移
    void StartFlee();
    void StartTaunt();
    void StopTaunt();
    void StopTauntToWait(float waitTime);

    // ダメージ復帰
    void BackToDamageRoot() override;
    std::unique_ptr<BaseEnemyBehavior> CreatePostSpawnBehavior() override;

protected:
    // 取り巻きが定位置に着けるよう、フィールド境界より内側に移動範囲を制限する
    void MoveToLimit() override;

private:
    // 初期化
    void InitAnimations();
    void InitTauntFont();
    void InitBombManager();

    // 煽り演出再生
    void PlayTauntFontSpawn();
    void PlayTauntFontMoving();
    void PlayTauntFontClose();
    void CreateAndSetupTauntBehavior();

    // 煽り文字の色演出
    static Vector3 RandomBrightColor();
    void StartNextColorTransition();

private:
    bool isTaunting_        = false;
    bool isTauntFontMoving_ = false;
    bool isFleeing_         = false;
    StrongParameter strongParam_;

    // スポーン時の初期位置
    Vector3 initialPosition_;

    // Taunt演出用フォントオブジェクト
    std::unique_ptr<KetaEngine::Object3d> tauntFont_;

    // 攻撃ボム管理
    std::unique_ptr<BossAttackBombManager> bombManager_;

    KetaEngine::Easing<Vector3> colorEasing_;
    Vector3 currentFontColor_ = {1.0f, 1.0f, 1.0f};

public:
    // Getter
    const StrongParameter& GetStrongParameter() const { return strongParam_; }
    void SetStrongParameter(const StrongParameter& param) { strongParam_ = param; }
    bool IsTaunting() const { return isTaunting_; }
    bool IsFleeing() const { return isFleeing_; }
    KetaEngine::Object3d* GetTauntFont() const { return tauntFont_.get(); }
    const Vector3& GetInitialPosition() const { return initialPosition_; }
};
