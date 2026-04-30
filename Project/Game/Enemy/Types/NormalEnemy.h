#pragma once
#include <list>
#include <memory>
// baseEnemy
#include "BaseEnemy.h"
// math
#include "Vector3.h"

/// <summary>
/// 通常敵クラス
/// </summary>
class NormalEnemy : public BaseEnemy {
public:
    enum class ZakoState {
        Spawning, // EnemySpawn実行中
        Flock,    // ボスを追従
        Taunt,    // プレイヤーを煽る
        Flee,     // ビビって逃げる
    };

public:
    NormalEnemy()           = default;
    ~NormalEnemy() override = default;

    ///========================================================================================
    ///  public method
    ///========================================================================================

    // 初期化、更新
    void Init(const Vector3& spownPos) override;
    void Update() override;

    // スポーン演出
    void SpawnRenditionInit() override;

    /// <summary>
    /// スポーン後のビヘイビアを生成
    /// </summary>
    std::unique_ptr<BaseEnemyBehavior> CreatePostSpawnBehavior() override;

    /// <summary>
    /// スプライト表示
    /// </summary>
    void DisplaySprite(const KetaEngine::ViewProjection& viewProjection) override;

    ///========================================================================================
    ///  煽り / 逃げ制御
    ///========================================================================================

    void StartTaunt(); //< 煽り開始
    void StopTaunt();  //< 煽り停止 → Flockに戻す
    void StartFlee();  //< 逃走開始

    ///========================================================================================
    ///  getter / setter
    ///========================================================================================
    ZakoState GetZakoState() const { return zakoState_; }
    bool IsTaunting() const { return zakoState_ == ZakoState::Taunt; }
    BaseEnemy* GetBoss() const { return pBoss_; }
    const Vector3& GetSpawnOffset() const { return spawnOffset_; }

    void SetBoss(BaseEnemy* boss) { pBoss_ = boss; }
    void SetSpawnOffset(const Vector3& offset) { spawnOffset_ = offset; }
    void SetSlot(int32_t index, int32_t count) { slotIndex_ = index; slotCount_ = count; }

    int32_t GetSlotIndex() const { return slotIndex_; }
    int32_t GetSlotCount() const { return slotCount_; }

private:
    BaseEnemy* pBoss_        = nullptr;
    Vector3    spawnOffset_  = {};
    ZakoState  zakoState_    = ZakoState::Spawning;
    int32_t    slotIndex_    = 0;
    int32_t    slotCount_    = 1;
};
