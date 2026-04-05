#include "TimelineRenderer.h"
#include "TimelineDrawer.h"

using namespace KetaEngine;
#include <cstdio>

void TimelineRenderer::Init(
    const TimelineDrawParameter* drawParam,
    const std::vector<TimeLineTrack>* tracks,
    const int32_t* currentFrame,
    const int* scrollOffset) {

    drawParam_    = drawParam;
    tracks_       = tracks;
    currentFrame_ = currentFrame;
    scrollOffset_ = scrollOffset;
}

void TimelineRenderer::DrawBackground(::ImDrawList* drawList, const Vector2& canvasPos, const Vector2& canvasSize) {
    // タイムラインの背景全体を塗りつぶす
    drawList->AddRectFilled(
        ImVec2(canvasPos.x, canvasPos.y),
        ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y),
        IM_COL32(30, 30, 30, 255));
}

void TimelineRenderer::DrawRulerAndGrid(::ImDrawList* drawList, const Vector2& canvasPos,
    const Vector2& canvasSize, float frameWidth, float trackAreaHeight,
    int visibleFrameStart, int visibleFrameEnd) {

    // ルーラー背景描画
    ImVec2 rulerStart = ImVec2(canvasPos.x, canvasPos.y);
    ImVec2 rulerEnd   = ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + drawParam_->GetRulerHeight());
    drawList->AddRectFilled(rulerStart, rulerEnd, IM_COL32(50, 50, 50, 255));

    // フレーム番号とグリッド
    for (int frame = visibleFrameStart; frame <= visibleFrameEnd && frame <= drawParam_->GetEndFrame(); frame++) {
        float x = canvasPos.x + drawParam_->GetHeaderWidth() + (frame - *scrollOffset_) * frameWidth;

        // 5フレームごとに太い線を描画し、フレーム番号を表示
        if (frame % 5 == 0) {
            // グリッド線の描画
            drawList->AddLine(
                ImVec2(x, canvasPos.y + drawParam_->GetRulerHeight()),
                ImVec2(x, canvasPos.y + trackAreaHeight),
                IM_COL32(60, 60, 60, 255), 1.0f);

            // フレーム番号の描画
            char frameText[8];
            std::snprintf(frameText, sizeof(frameText), "%d", frame);
            drawList->AddText(ImVec2(x + 2, canvasPos.y + 5), IM_COL32(200, 200, 200, 255), frameText);
        } else {
            // 1フレームごとに細い線を描画
            drawList->AddLine(
                ImVec2(x, canvasPos.y + drawParam_->GetRulerHeight()),
                ImVec2(x, canvasPos.y + trackAreaHeight),
                IM_COL32(45, 45, 45, 255), 1.0f);
        }
    }
}

void TimelineRenderer::DrawTracks(::ImDrawList* drawList, const Vector2& canvasPos,
    const Vector2& canvasSize, float frameWidth,
    int visibleFrameStart, int visibleFrameEnd) {

    // トラックの描画開始Y位置(ルーラーから下)
    float trackY = canvasPos.y + drawParam_->GetRulerHeight();

    // 各トラックの描画
    for (uint32_t i = 0; i < tracks_->size(); i++) {
        const TimeLineTrack& track = (*tracks_)[i];

        // トラックヘッダーの始点終点
        ImVec2 headerStart = ImVec2(canvasPos.x, trackY);
        ImVec2 headerEnd   = ImVec2(canvasPos.x + drawParam_->GetHeaderWidth(), trackY + drawParam_->GetTrackHeight());

        // ヘッダー背景と枠線、テキスト描画
        drawList->AddRectFilled(headerStart, headerEnd, IM_COL32(40, 40, 40, 255)); // 背景
        drawList->AddRect(headerStart, headerEnd, IM_COL32(70, 70, 70, 255));       // 枠線
        drawList->AddText(ImVec2(canvasPos.x + 10, trackY + 8), IM_COL32(255, 255, 255, 255), track.name.c_str()); // 文字

        // トラックライン背景の始点終点
        ImVec2 lineStart = ImVec2(canvasPos.x + drawParam_->GetHeaderWidth(), trackY);
        ImVec2 lineEnd   = ImVec2(canvasPos.x + canvasSize.x, trackY + drawParam_->GetTrackHeight());

        // ヘッダー背景と枠線描画
        drawList->AddRectFilled(lineStart, lineEnd, (i % 2 == 0) ? IM_COL32(35, 35, 35, 255) : IM_COL32(30, 30, 30, 255));
        drawList->AddLine(ImVec2(canvasPos.x, trackY + drawParam_->GetTrackHeight()),
            ImVec2(canvasPos.x + canvasSize.x, trackY + drawParam_->GetTrackHeight()), IM_COL32(50, 50, 50, 255));

        // キーフレーム視覚部分の描画
        for (uint32_t j = 0; j < track.keyframes.size(); j++) {
            DrawKeyFrameVisual(drawList, i, j, canvasPos, frameWidth, visibleFrameStart, visibleFrameEnd, trackY);
        }

        // 次のトラックのY位置へ加算
        trackY += drawParam_->GetTrackHeight();
    }
}

void TimelineRenderer::DrawKeyFrameVisual(::ImDrawList* drawList, uint32_t trackIndex, uint32_t keyIndex,
    const Vector2& canvasPos, float frameWidth,
    int visibleFrameStart, int visibleFrameEnd, float trackY) {

    const TimeLineTrack&    track = (*tracks_)[trackIndex];
    const TimeLineKeyFrame& kf    = track.keyframes[keyIndex];

    if (kf.frame < visibleFrameStart || kf.frame > visibleFrameEnd) {
        return;
    }

    float kfX = canvasPos.x + drawParam_->GetHeaderWidth() + (kf.frame - *scrollOffset_) * frameWidth;
    float kfY = trackY + drawParam_->GetTrackHeight() / 2;

    bool isOnCurrentFrame = (*currentFrame_ >= kf.frame && *currentFrame_ <= kf.frame + static_cast<int32_t>(kf.duration));

    ImU32 color;
    ImU32 durationUIColor;
    if (isOnCurrentFrame) {
        color           = IM_COL32(70, 255, 70, 255);
        durationUIColor = IM_COL32(100, 255, 200, 180);
    } else {
        color           = IM_COL32(255, 200, 50, 255);
        durationUIColor = IM_COL32(100, 150, 200, 180);
    }

    // 持続時間の表示
    if (kf.duration > 1.0f) {
        float durationWidth = kf.duration * frameWidth;
        float barHeight     = drawParam_->GetTrackHeight() * 0.8f;
        float barTopY       = trackY + (drawParam_->GetTrackHeight() - barHeight) / 2.0f;

        drawList->AddRectFilled(
            ImVec2(kfX, barTopY),
            ImVec2(kfX + durationWidth, barTopY + barHeight),
            durationUIColor);

        // ラベル表示
        if (!kf.label.empty()) {
            drawList->AddText(
                ImVec2(kfX + 5, barTopY + 2),
                IM_COL32(255, 255, 255, 255),
                kf.label.c_str());
        }
    }

    // キーフレームマーカー(ダイヤモンド)
    ImVec2 diamond[4] = {
        ImVec2(kfX, kfY - 6),
        ImVec2(kfX + 6, kfY),
        ImVec2(kfX, kfY + 6),
        ImVec2(kfX - 6, kfY)};

    drawList->AddConvexPolyFilled(diamond, 4, color);

    // ホバー時のラベル表示
    if (!kf.label.empty() && ImGui::IsMouseHoveringRect(ImVec2(kfX - 8, kfY - 8), ImVec2(kfX + 8, kfY + 8))) {
        ImGui::SetTooltip("%s", kf.label.c_str());
    }
}

void TimelineRenderer::DrawEndFrameLine(::ImDrawList* drawList, const Vector2& canvasPos,
    float frameWidth, float trackAreaHeight) {

    float endFrameX  = canvasPos.x + drawParam_->GetHeaderWidth() + (drawParam_->GetEndFrame() - *scrollOffset_) * frameWidth;
    float canvasEndX = canvasPos.x + drawParam_->GetHeaderWidth();

    if (endFrameX < canvasEndX) {
        return;
    }

    float dashLength = 5.0f;
    float gapLength  = 3.0f;
    float currentY   = canvasPos.y;

    while (currentY < canvasPos.y + trackAreaHeight) {
        float nextY = currentY + dashLength;
        if (nextY > canvasPos.y + trackAreaHeight) {
            nextY = canvasPos.y + trackAreaHeight;
        }

        drawList->AddLine(
            ImVec2(endFrameX, currentY),
            ImVec2(endFrameX, nextY),
            IM_COL32(255, 50, 50, 255), 2.0f);

        currentY = nextY + gapLength;
    }
}

void TimelineRenderer::DrawPlayHead(::ImDrawList* drawList, const Vector2& canvasPos,
    float frameWidth, float trackAreaHeight) {

    // 現在フレームインジケーター（赤い縦線）
    float currentFrameX = canvasPos.x + drawParam_->GetHeaderWidth() + (*currentFrame_ - *scrollOffset_) * frameWidth;
    drawList->AddLine(
        ImVec2(currentFrameX, canvasPos.y),
        ImVec2(currentFrameX, canvasPos.y + trackAreaHeight),
        IM_COL32(255, 100, 100, 255), 2.0f);
}
