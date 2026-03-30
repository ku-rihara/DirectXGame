#pragma once

#include "KillBonusComboUI.h"
#include "KillBonusSimKillUI.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "Vector2.h"
#include <cstdint>
#include <functional>
#include <string>

class KillBonusController {
public:
    KillBonusController()  = default;
    ~KillBonusController() = default;

    void Init();
    void Update(float deltaTime);
    void AdjustParam();
    void RegisterParams();

    /// 敵撃破イベント（コンボ倍率、同時キル判定）
    /// 敵撃破イベント（同時キル判定のみ、EnemyManagerから呼ぶ）
    void OnEnemyKilled(int32_t comboCount);

    /// BonusFly到着イベント（コンボUIスポーン、flyコールバックから呼ぶ）
    void OnBonusFlyArrived(float comboBonusValue, int32_t comboCount);

    void SetOnSimKillBonusCallback(std::function<void(float)> cb) { onSimKillBonusCallback_ = std::move(cb); }

private:
    void FlushSimKillWindow();
    void ResetDisplayTimer();
    void SpawnPreview();

private:
    KillBonusComboUI   comboUI_;
    KillBonusSimKillUI simKillUI_;

    KillBonusComboUILayout   comboLayout_;
    KillBonusSimKillUILayout simKillLayout_;

    // 共有表示タイマー
    float displayTimer_    = 0.0f;
    float displayDuration_ = 2.0f;
    bool  displayActive_   = false;

    // 同時キル判定
    struct SimKillTracker {
        float   toleranceTime = 0.0f;
        int32_t killCount     = 0;
    };
    SimKillTracker simKillTracker_;
    float simKillToleranceTime_ = 0.1f;
    float simKillBonusPerKill_  = 0.5f;

    float   lastComboBonusValue_ = 1.0f;
    int32_t pendingSimKillCount_ = 0;  // fly到着まで保留する同時キル数

    // デバッグプレビュー
    bool    previewActive_          = false;
    float   previewComboBonusValue_ = 2.5f;
    bool    previewHasSimKill_      = true;
    int32_t previewSimKillCount_    = 3;

    KetaEngine::GlobalParameter* globalParameter_ = nullptr;
    const std::string groupName_ = "KillBonusUI";

    std::function<void(float)> onSimKillBonusCallback_;
};
