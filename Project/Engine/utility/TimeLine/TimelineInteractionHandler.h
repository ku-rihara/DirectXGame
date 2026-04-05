#pragma once
#include "TimelineDrawParameter.h"
#include "Vector2.h"
#include <cstdint>
#include <functional>
#include <vector>

namespace KetaEngine {

// 前方宣言
struct TimeLineTrack;

/// <summary>
/// タイムラインのUI処理を担当するクラス
/// </summary>
class TimelineInteractionHandler {
public:
    TimelineInteractionHandler()  = default;
    ~TimelineInteractionHandler() = default;

    //*----------------------------- public Methods -----------------------------*//

    /// <summary>
    /// 初期化
    /// </summary>
    void Init(
        TimelineDrawParameter* drawParam,
        std::vector<TimeLineTrack>* tracks);

    /// <summary>
    /// ドラッグ状態をリセット
    /// </summary>
    void Reset();

    /// <summary>
    /// トラック削除後にドラッグ中インデックスを調整
    /// </summary>
    void AdjustIndicesAfterTrackRemoval(int32_t removedTrackIndex);

    // キーフレームのドラッグ＆ドロップ処理
    void HandleKeyFrameDragDrop(uint32_t trackIndex, uint32_t keyIndex,
        const Vector2& keyPos);

    // デュレーションバーのドラッグ処理
    void HandleDurationDrag(uint32_t trackIndex, uint32_t keyIndex,
        float durationBarX, float trackY);

    // ドラッグ状態の更新
    void HandleDragUpdates(const Vector2& canvasPos, float frameWidth);

    // キャンバス操作処理
    void HandleCanvasInteraction(const Vector2& canvasPos, const Vector2& canvasSize,
        float frameWidth, float trackAreaHeight,
        const std::function<void()>& applyCurrentFrameCallback);

    // 再生状態更新
    void UpdatePlayback(const std::function<void()>& applyCurrentFrameCallback);

private:
    //*---------------------------- private Variant ----------------------------*//

    // ドラッグ対象（トラックとキーフレームのインデックスペア）
    struct DragTarget {
        int32_t trackIndex = -1; // 無効値
        int32_t keyIndex   = -1; // 無効値
    };

    TimelineDrawParameter* drawParam_   = nullptr;
    std::vector<TimeLineTrack>* tracks_ = nullptr;

    int32_t currentFrame_ = 0;
    int scrollOffset_     = 0;
    bool isPlaying_       = false;

    // キーフレームドラッグ用
    DragTarget draggingKeyFrame_;
    int dragStartFrame_ = 0;

    // duration ドラッグ用
    DragTarget draggingDuration_;
    float dragStartDuration_ = 0.0f;

    // 再生ヘッド（縦線）ドラッグ用
    bool isDraggingPlayHead_ = false;

public:
    //*----------------------------- getter / setter -----------------------------*//

    int32_t GetCurrentFrame() const { return currentFrame_; }
    int GetScrollOffset() const { return scrollOffset_; }
    bool GetIsPlaying() const { return isPlaying_; }

    void SetCurrentFrame(int32_t f) { currentFrame_ = f; }
    void SetScrollOffset(int s) { scrollOffset_ = s; }
    void SetIsPlaying(bool p) { isPlaying_ = p; }
};

} // KetaEngine
