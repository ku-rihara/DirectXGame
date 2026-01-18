#pragma once

#include "utility/TimeLine/TimeLine.h"
#include <array>
#include <cstdint>
#include <memory>
#include <vector>

class PlayerComboAttackData;
/// <summary>
/// プレイヤー攻撃用タイムラインエディタ
/// </summary>
class PlayerComboAttackTimeline {
public:
    enum class PlayMode {
        SINGLE, // 単体再生
        CONTINUOUS // 連続再生
    };

    enum class DefaultTrack {
        MOVE_EASING,
        CANCEL_START,
        PRECEDE_INPUT_START,
        COUNT
    };

public:
    PlayerComboAttackTimeline()  = default;
    ~PlayerComboAttackTimeline() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    void Init(PlayerComboAttackData* attackData);

    /// <summary>
    /// ImGui描画
    /// </summary>
    void Draw();

    /// <summary>
    /// タイムラインから攻撃パラメータへ反映
    /// </summary>
    void ApplyToParameters();

private:
    /// <summary>
    /// デフォルトトラックのセットアップ
    /// </summary>
    void SetupDefaultTracks();

    /// <summary>
    /// 演出トラックのセットアップ
    /// </summary>
    void SetupRenditionTracks();

    /// <summary>
    /// オブジェクトアニメーショントラックのセットアップ
    /// </summary>
    void SetupObjectAnimationTracks();

    /// <summary>
    /// オーディオトラックのセットアップ
    /// </summary>
    void SetupAudioTracks();

    /// <summary>
    /// 攻撃の総フレーム数を計算
    /// </summary>
    int32_t CalculateTotalFrames() const;

    /// <summary>
    /// 次の攻撃へ進む
    /// </summary>
    void AdvanceToNextAttack();

private:
    PlayerComboAttackData* attackData_ = nullptr;
    KetaEngine::TimeLine timeline_;

    // トラックインデックス管理
    std::array<int32_t, static_cast<size_t>(DefaultTrack::COUNT)> defaultTrackIndices_;
    std::vector<int32_t> renditionTrackIndices_;
    std::vector<int32_t> objAnimeTrackIndices_;
    std::vector<int32_t> audioTrackIndices_;

    PlayMode playMode_  = PlayMode::SINGLE;
    bool isInitialized_ = false;
};