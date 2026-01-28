#pragma once

#include "Vector3.h"
#include <cstdint>
#include <memory>
#include <string>

class Player;
class PlayerComboAttackData;
class PlayerComboAttackController;

namespace KetaEngine {
class TimelineDrawer;
}

enum class AttackPreviewMode {
    NONE, // プレビュー無効
    SINGLE, // 単体再生
    CONTINUOUS, // 連続再生
};

/// <summary>
/// タイムラインでの攻撃プレビュークラス
/// </summary>
class PlayerComboAttackPreview {
public:
    PlayerComboAttackPreview()  = default;
    ~PlayerComboAttackPreview() = default;

    // 初期化
    void Init(Player* player,
        PlayerComboAttackController* controller,
        KetaEngine::TimelineDrawer* timeline);

    // 更新
    void Update(float deltaTime);

    // UI描画
    void DrawUI();

    // プレビュー制御
    void StartPreview(PlayerComboAttackData* attackData, AttackPreviewMode mode);
    void StopPreview();

private:
    // プレビュー更新処理
    void UpdateSingleMode();
    void UpdateContinuousMode();

    // 攻撃実行制御
    void ExecuteAttack(PlayerComboAttackData* attackData);
    void CheckAndAdvanceToNextAttack();

    // 入力シミュレーション
    bool ShouldTriggerNextAttack();

    // 状態リセット
    void ResetState();
    void SaveInitialState();
    void RestoreInitialState();

    PlayerComboAttackData* FindFirstAttackInChain(PlayerComboAttackData* currentAttack);

private:
    Player* player_                                = nullptr;
    PlayerComboAttackController* attackController_ = nullptr;
    KetaEngine::TimelineDrawer* timeline_          = nullptr;

    AttackPreviewMode currentMode_ = AttackPreviewMode::NONE;

    // 現在のプレビュー状態
    PlayerComboAttackData* currentAttackData_ = nullptr;
    std::string nextAttackName_;

    // タイマー
    float attackElapsedTime_     = 0.0f;
    float buttonInputTimer_      = 0.0f;
    int32_t buttonInputInterval_ = 5; // ボタン連打間隔（フレーム）

    // フラグ
    bool hasSimulatedInput_   = false;
    bool isWaitingTransition_ = false;

    // プレイヤーの初期状態保存
    Vector3 initialPosition_;
    Vector3 initialRotation_;

public:
    // Getter
    AttackPreviewMode GetCurrentMode() const { return currentMode_; }
    bool IsPlaying() const { return currentMode_ != AttackPreviewMode::NONE; }
    int32_t GetButtonInputInterval() const { return buttonInputInterval_; }
    std::string GetCurrentAttackName() const;

    // Setter
    void SetButtonInputInterval(int32_t interval) { buttonInputInterval_ = interval; }
    void SetPlayMode(AttackPreviewMode mode) { currentMode_ = mode; }
};