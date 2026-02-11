#pragma once

#include "PlayerComboAttackTimelineData.h"
#include "utility/TimeLine/TimelineDrawer.h"
#include <cstdint>
#include <optional>

class PlayerComboAttackData;

/// <summary>
/// タイムラインの変更をパラメータに適用するクラス
/// </summary>
class PlayerComboAttackTimelineParameterApplier {
public:
    // キーフレーム構造体
    struct KeyFrameInfo {
        int32_t startFrame; // 開始フレーム位置
        float duration; // キーフレームの持続時間（フレーム数）
    };

public:
    PlayerComboAttackTimelineParameterApplier()  = default;
    ~PlayerComboAttackTimelineParameterApplier() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="attackData">攻撃データへのポインタ</param>
    /// <param name="timeline">タイムラインDrawerへのポインタ</param>
    /// <param name="data">タイムラインデータへのポインタ</param>
    void Init(PlayerComboAttackData* attackData,
        KetaEngine::TimelineDrawer* timeline,
        PlayerComboAttackTimelineData* data);

    /// <summary>
    /// タイムラインの情報を攻撃パラメータに適用
    /// </summary>
    void ApplyToParameters();

    /// <summary>
    /// 終了待機時間のキーフレーム位置を更新
    /// </summary>
    void UpdateFinishWaitKeyFramePosition();

    /// <summary>
    /// タイムライン終了フレームを設定
    /// </summary>
    void UpdateTimelineEndFrame();

private:
    /// <summary>
    /// トラックからキーフレーム情報（開始フレーム+継続時間）を取得
    /// トラックが存在しない、またはキーフレームが空の場合はnulloptを返す
    /// </summary>
    /// <param name="track">取得したいデフォルトトラックの種類</param>
    /// <returns>キーフレーム情報、存在しない場合はnullopt</returns>
    std::optional<KeyFrameInfo> GetKeyFrameInfo(
        PlayerComboAttackTimelineData::DefaultTrack track) const;

    /// <summary>
    /// トラックから単一のフレーム値を取得
    /// キャンセルタイムや先行入力など、デュレーションが不要な場合に使用
    /// </summary>
    /// <param name="track">取得したいデフォルトトラックの種類</param>
    /// <returns>フレーム値、存在しない場合はnullopt</returns>
    std::optional<int32_t> GetSingleFrameValue(
        PlayerComboAttackTimelineData::DefaultTrack track) const;

    /// <summary>
    /// 演出系トラックの情報をRenditionパラメータに適用
    /// </summary>
    /// <param name="trackInfo">トラック情報</param>
    /// <param name="timing">開始タイミング（秒）</param>
    void ApplyTrackToRendition(
        const PlayerComboAttackTimelineData::TrackInfo& trackInfo,
        float timing);

    /// <summary>
    /// 終了待機時間の開始フレームを取得
    /// コリジョン終了時間と移動終了時間の遅い方を返す
    /// </summary>
    /// <returns>終了待機開始フレーム</returns>
    int32_t GetFinishWaitStartFrame() const;

    /// <summary>
    /// タイムラインの総フレーム数を計算
    /// 移動開始～移動終了～終了待機終了までの合計
    /// </summary>
    /// <returns>総フレーム数</returns>
    int32_t CalculateTotalFrames() const;

private:
    PlayerComboAttackData* attackData_           = nullptr; // 攻撃データ
    KetaEngine::TimelineDrawer* timelineDrawer_  = nullptr; // タイムラインDrawer
    PlayerComboAttackTimelineData* timeLineData_ = nullptr; // タイムラインデータ管理
};