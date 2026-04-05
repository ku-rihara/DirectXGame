#pragma once
#include "TimelineDrawParameter.h"
#include "Vector2.h"
#include <cstdint>
#include <imgui.h>
#include <vector>

namespace KetaEngine {

// 前方宣言
struct TimeLineKeyFrame;
struct TimeLineTrack;

/// <summary>
/// タイムラインの描画処理を担当するクラス
/// </summary>
class TimelineRenderer {
public:
    TimelineRenderer()  = default;
    ~TimelineRenderer() = default;

    //*----------------------------- public Methods -----------------------------*//

    /// <summary>
    /// 初期化
    /// </summary>
    void Init(
        const TimelineDrawParameter* drawParam,
        const std::vector<TimeLineTrack>* tracks,
        const int32_t* currentFrame,
        const int* scrollOffset);

    /// <summary>
    /// 背景描画
    /// </summary>
    void DrawBackground(::ImDrawList* drawList, const Vector2& canvasPos,
        const Vector2& canvasSize);

    /// <summary>
    /// ルーラー＆グリッド描画
    /// </summary>
    void DrawRulerAndGrid(::ImDrawList* drawList, const Vector2& canvasPos,
        const Vector2& canvasSize, float frameWidth, float trackAreaHeight,
        int visibleFrameStart, int visibleFrameEnd);

    /// <summary>
    /// トラック全体描画（背景・ヘッダー・キーフレーム視覚部分）
    /// </summary>
    void DrawTracks(::ImDrawList* drawList, const Vector2& canvasPos,
        const Vector2& canvasSize, float frameWidth,
        int visibleFrameStart, int visibleFrameEnd);

    /// <summary>
    /// 終了フレームライン描画
    /// </summary>
    void DrawEndFrameLine(::ImDrawList* drawList, const Vector2& canvasPos,
        float frameWidth, float trackAreaHeight);

    /// <summary>
    /// 再生ヘッド（現在フレームの縦線）描画
    /// </summary>
    void DrawPlayHead(::ImDrawList* drawList, const Vector2& canvasPos,
        float frameWidth, float trackAreaHeight);

private:
    //*---------------------------- Draw Helper Methods ----------------------------*//

    // キーフレームの視覚部分のみを描画
    void DrawKeyFrameVisual(::ImDrawList* drawList, uint32_t trackIndex, uint32_t keyIndex,
        const Vector2& canvasPos, float frameWidth,
        int visibleFrameStart, int visibleFrameEnd, float trackY);

private:
    //*---------------------------- private Variant ----------------------------*//

    const TimelineDrawParameter* drawParam_   = nullptr;
    const std::vector<TimeLineTrack>* tracks_ = nullptr;
    const int32_t* currentFrame_              = nullptr;
    const int* scrollOffset_                  = nullptr;
};

} // KetaEngine
