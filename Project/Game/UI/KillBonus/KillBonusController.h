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

    // 敵撃破イベント（comboBonusValue: コンボ倍率float）
    void OnEnemyKilled(float comboBonusValue);

    void SetOnSimKillBonusCallback(std::function<void(float)> cb) { onSimKillBonusCallback_ = std::move(cb); }

private:
    void SpawnEntry(float comboBonusValue);
    void RecalculateTargetPositions();
    void FlushSimKillWindow();

    // デバッグプレビュー
    void SpawnPreview();

private:
    bool  previewActive_          = false;
    float previewComboBonusValue_ = 2.0f;
    bool  previewHasSimKill_      = true;
    int32_t previewSimKillValue_  = 2;

private:
    // 同時キル検出
    struct SimKillTracker {
        float             toleranceTime = 0.0f; // 残り時間（秒）
        int32_t           killCount     = 0;    // ウィンドウ内キル数
        std::vector<int>  entryIndices;          // ウィンドウ内エントリのインデックス
    };

    std::vector<std::unique_ptr<KillBonusEntry>> entries_;
    SimKillTracker              simKillTracker_;

    KillBonusLayoutParam layout_;

    Vector2 basePos_;
    Vector2 entryOffset_;

    int32_t maxEntries_             = 3;    // 最大同時表示数（超えたら最古をその場でクローズ）
    float   simKillToleranceTime_   = 0.1f; // 同時キル判定時間（秒）
    float   simKillBonusPerKill_    = 0.5f; // 1キルあたりのボーナスゲージ量

    KetaEngine::GlobalParameter* globalParameter_;
    const std::string groupName_ = "KillBonusUI";

    std::function<void(float)> onSimKillBonusCallback_;
};
