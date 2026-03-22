#pragma once

#include "Player/ComboCreator/PlayerAttackRenditionData.h"
#include "Vector3.h"
#include <array>
#include <cstdint>
#include <memory>
#include <string>

class Player;
class PlayerComboAttackData;
class PlayerComboAttackController;

namespace KetaEngine {
class TimelineDrawer;
class EffectEditorSuite;
}

enum class AttackPreviewMode {
    NONE,   // プレビュー無効
    SINGLE, // 単体再生
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

    // 攻撃実行制御
    void ExecuteAttack(PlayerComboAttackData* attackData);

    // 演出再生（EffectEditor経由）
    void UpdatePreviewRenditions();
    void ResetRenditionFlags();

    // 状態リセット
    void ResetState();
    void SaveInitialState();
    void RestoreInitialState();

private:
    Player* player_                                = nullptr;
    PlayerComboAttackController* attackController_ = nullptr;
    KetaEngine::TimelineDrawer* timeline_          = nullptr;
    KetaEngine::EffectEditorSuite* pEditorSuite_   = nullptr;

    AttackPreviewMode currentMode_ = AttackPreviewMode::NONE;

    // 現在のプレビュー状態
    PlayerComboAttackData* currentAttackData_ = nullptr;

    // タイマー
    float attackElapsedTime_ = 0.0f;

    // 原点スタートフラグ
    bool startFromOrigin_ = false;
    Vector3 previewInitPos_;

    // 演出再生フレーム追跡
    int32_t prevFrame_ = 0;
    std::array<bool, static_cast<size_t>(PlayerAttackRenditionData::Type::Count)> renditionPlayed_{};
    std::array<bool, static_cast<size_t>(PlayerAttackRenditionData::ObjAnimationType::Count)> objAnimPlayed_{};

    // プレイヤーの初期状態保存
    Vector3 initialPosition_;
    Vector3 initialRotation_;

public:
    // Getter
    AttackPreviewMode GetCurrentMode() const { return currentMode_; }
    bool IsPlaying() const { return currentMode_ != AttackPreviewMode::NONE; }
    std::string GetCurrentAttackName() const;

    // Setter
    void SetEditorSuite(KetaEngine::EffectEditorSuite* suite) { pEditorSuite_ = suite; }
};