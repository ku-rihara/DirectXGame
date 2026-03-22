#pragma once

#include "KillBonusEntry.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "Vector2.h"
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

/// キルボーナスUI管理クラス
class KillBonusController {
public:
    KillBonusController()  = default;
    ~KillBonusController() = default;

    void Init();
    void Update(float deltaTime);

    void AdjustParam();
    void RegisterParams();

    // 敵撃破イベント（comboMultiplier: 表示用整数倍率）
    void OnEnemyKilled(int32_t comboMultiplier);

    void SetOnSimKillBonusCallback(std::function<void(float)> cb) { onSimKillBonusCallback_ = std::move(cb); }

private:
    void SpawnEntry(int32_t comboMultiplier);
    void RecalculateTargetPositions();
    void FlushSimKillWindow();

private:
    // 同時キル検出
    struct SimKillTracker {
        int32_t           toleranceFrames = 0;  // 残りフレーム
        int32_t           killCount       = 0;  // ウィンドウ内キル数
        std::vector<int>  entryIndices;          // ウィンドウ内エントリのインデックス
    };

    std::vector<std::unique_ptr<KillBonusEntry>> entries_;
    SimKillTracker              simKillTracker_;

    KillBonusLayoutParam layout_;

    Vector2 basePos_;
    Vector2 entryOffset_;

    int32_t simKillToleranceFrames_ = 6;   // 同時キル判定フレーム数
    float   simKillBonusPerKill_    = 0.5f; // 1キルあたりのボーナスゲージ量

    KetaEngine::GlobalParameter* globalParameter_;
    const std::string groupName_ = "KillBonusUI";

    std::function<void(float)> onSimKillBonusCallback_;
};
