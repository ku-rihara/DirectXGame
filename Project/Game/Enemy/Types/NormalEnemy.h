#pragma once
#include <list>
#include <memory>
// baseEnemy
#include "BaseEnemy.h"
// math
#include "Vector3.h"
#include <string>

/// <summary>
/// 通常敵クラス
/// </summary>
class NormalEnemy : public BaseEnemy {
public:
    enum class ZakoState {
        Spawning, // EnemySpawn実行中
        Flock, // ボスを追従
        Taunt, // プレイヤーを煽る
        CrawlBackwards, // 後ずさりする
    };

    // NormalEnemy固有アニメーション
    enum class NormalAnimationType {
        StumbleBackwards,
        CrawlBackwards,
        Count
    };

    struct NormalParameter {
        float fleeSpeed = 5.0f;
        float fleeTime  = 6.0f;
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

   
    /// Behavior
    void StartTaunt(); //< 煽り開始
    void StopTaunt(); //< 煽り停止 → Flockに戻す
    void StartFlee(); //< 逃走開始

    /// <summary>
    /// ダメージリアクション終了後にRootへ戻る
    /// </summary>
    void BackToDamageRoot() override;

    void AddNormalAnimation(NormalAnimationType type, const std::string& name);
    bool PlayNormalAnimation(NormalAnimationType type, bool isLoop = false);

private:
    std::array<std::string, static_cast<size_t>(NormalAnimationType::Count)> normalAnimationNames_;

    BaseEnemy* pBoss_    = nullptr;
    Vector3 spawnOffset_ = {};
    ZakoState zakoState_ = ZakoState::Spawning;
    int32_t slotIndex_   = 0;
    int32_t slotCount_   = 1;

    std::string parentBossName_;
    NormalParameter normalParam_;

    // StumbleBackwards再生中フラグ
    bool isInStumblePhase_ = false;

public:
    ///========================================================================================
    ///  getter / setter
    ///========================================================================================
    ZakoState GetZakoState() const { return zakoState_; }
    bool IsTaunting() const { return zakoState_ == ZakoState::Taunt; }
    BaseEnemy* GetBoss() const { return pBoss_; }
    const Vector3& GetSpawnOffset() const { return spawnOffset_; }
    const std::string& GetParentBossName() const { return parentBossName_; }
    const NormalParameter& GetNormalParameter() const { return normalParam_; }

    void SetBoss(BaseEnemy* boss) { pBoss_ = boss; }
    void SetSpawnOffset(const Vector3& offset) { spawnOffset_ = offset; }
    void SetSlot(int32_t index, int32_t count);
    void SetParentBossName(const std::string& name) { parentBossName_ = name; }
    void SetNormalParameter(const NormalParameter& param) { normalParam_ = param; }

    int32_t GetSlotIndex() const { return slotIndex_; }
    int32_t GetSlotCount() const { return slotCount_; }

    bool IsInStumblePhase() const { return isInStumblePhase_; }
    void SetIsInStumblePhase(bool value) { isInStumblePhase_ = value; }
};