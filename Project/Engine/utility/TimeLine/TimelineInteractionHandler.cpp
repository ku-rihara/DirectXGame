#include "TimelineInteractionHandler.h"
#include "TimelineDrawer.h" 

using namespace KetaEngine;
#include <algorithm>

void TimelineInteractionHandler::Init(
    TimelineDrawParameter* drawParam,
    std::vector<TimeLineTrack>* tracks) {

    drawParam_ = drawParam;
    tracks_    = tracks;
}

void TimelineInteractionHandler::Reset() {
    draggingKeyFrame_ = {};
    draggingDuration_ = {};
    dragStartFrame_     = 0;
    dragStartDuration_  = 0.0f;
    isDraggingPlayHead_ = false;
}

void TimelineInteractionHandler::AdjustIndicesAfterTrackRemoval(int32_t removedTrackIndex) {
    // ドラッグ中のトラックや右クリックされたトラックのインデックスを調整
    if (draggingKeyFrame_.trackIndex == removedTrackIndex) {
        draggingKeyFrame_ = {};
    } else if (draggingKeyFrame_.trackIndex > removedTrackIndex) {
        // 削除されたトラック以降のインデックスを-1ずらす
        draggingKeyFrame_.trackIndex--;
    }

    // duration ドラッグ中のトラックも同様に調整
    if (draggingDuration_.trackIndex == removedTrackIndex) {
        draggingDuration_ = {};
    } else if (draggingDuration_.trackIndex > removedTrackIndex) {
        draggingDuration_.trackIndex--;
    }
}

void TimelineInteractionHandler::HandleKeyFrameDragDrop(uint32_t trackIndex, uint32_t keyIndex, const Vector2& keyPos) {
    // 位置ロック中のキーフレームはドラッグ不可
    if ((*tracks_)[trackIndex].keyframes[keyIndex].isPositionLocked) {
        return;
    }

    // 矩形の半分のサイズ
    const float rectSize = 8.0f; 

    // キーフレームをホバー中かの判定
    if (ImGui::IsMouseClicked(0) && ImGui::IsMouseHoveringRect(ImVec2(keyPos.x - rectSize, keyPos.y - rectSize), ImVec2(keyPos.x + rectSize, keyPos.y + rectSize))) {

        // キーフレームドラッグ開始処理
        draggingKeyFrame_.trackIndex                           = trackIndex;
        draggingKeyFrame_.keyIndex                             = keyIndex;
        dragStartFrame_                                        = (*tracks_)[trackIndex].keyframes[keyIndex].frame;
        (*tracks_)[trackIndex].keyframes[keyIndex].isSelected  = true;
    }
}

void TimelineInteractionHandler::HandleDurationDrag(uint32_t trackIndex, uint32_t keyIndex, float durationBarX, float trackY) {
    // デュレーションバーのドラッグ処理
    const float barHeight     = drawParam_->GetTrackHeight();
    const float barTopY       = trackY + (drawParam_->GetTrackHeight() - barHeight) / 2.0f;
    const float dragZoneWidth = 8.0f;

    // 　ドラッグ判定をデュレーションバーの左右端に配置
    ImVec2 dragZoneMin = ImVec2(durationBarX - dragZoneWidth, barTopY);
    ImVec2 dragZoneMax = ImVec2(durationBarX + dragZoneWidth, barTopY + barHeight);

    // デュレーションバードラッグ開始処理
    if (ImGui::IsMouseClicked(0) && ImGui::IsMouseHoveringRect(dragZoneMin, dragZoneMax)) {
        draggingDuration_.trackIndex = trackIndex;
        draggingDuration_.keyIndex   = keyIndex;
        dragStartDuration_           = (*tracks_)[trackIndex].keyframes[keyIndex].duration;
    }
}

void TimelineInteractionHandler::HandleDragUpdates(const Vector2& canvasPos, float frameWidth) {
    // ドラッグ中の処理(キーフレーム位置)
    if (draggingKeyFrame_.trackIndex >= 0 && ImGui::IsMouseDragging(0)) {
        ImVec2 mousePos = ImGui::GetMousePos();
        // 開始位置からのフレームの変化量を計算
        float deltaX   = mousePos.x - (canvasPos.x + drawParam_->GetHeaderWidth() + (dragStartFrame_ - scrollOffset_) * frameWidth);
        int deltaFrame = static_cast<int>(deltaX / frameWidth);
        int newFrame   = dragStartFrame_ + deltaFrame;

        // 動かしたフレームを適応
        if (newFrame >= 0 && newFrame <= drawParam_->GetEndFrame()) {
            (*tracks_)[draggingKeyFrame_.trackIndex].keyframes[draggingKeyFrame_.keyIndex].frame = newFrame;
        }
    }

    // ドラッグ中の処理(duration)
    if (draggingDuration_.trackIndex >= 0 && ImGui::IsMouseDragging(0)) {
        ImVec2 mousePos   = ImGui::GetMousePos();
        auto& kf          = (*tracks_)[draggingDuration_.trackIndex].keyframes[draggingDuration_.keyIndex];
        float kfX         = canvasPos.x + drawParam_->GetHeaderWidth() + (kf.frame - scrollOffset_) * frameWidth;
        float deltaX      = mousePos.x - kfX;
        float newDuration = deltaX / frameWidth;

        if (newDuration >= 1.0f) {
            kf.duration = newDuration;
        }
    }

    // ドラッグ終了
    if (ImGui::IsMouseReleased(0)) {
        if (draggingKeyFrame_.trackIndex >= 0) {
            std::sort((*tracks_)[draggingKeyFrame_.trackIndex].keyframes.begin(),
                (*tracks_)[draggingKeyFrame_.trackIndex].keyframes.end(),
                [](const TimeLineKeyFrame& a, const TimeLineKeyFrame& b) { return a.frame < b.frame; });

            (*tracks_)[draggingKeyFrame_.trackIndex].keyframes[draggingKeyFrame_.keyIndex].isSelected = false;
        }
        draggingKeyFrame_ = {};
        draggingDuration_ = {};
    }
}

void TimelineInteractionHandler::HandleCanvasInteraction(const Vector2& canvasPos, const Vector2& canvasSize,
    float frameWidth, float trackAreaHeight,
    const std::function<void()>& applyCurrentFrameCallback) {

    trackAreaHeight;
    ImGui::SetCursorScreenPos(ImVec2(canvasPos.x, canvasPos.y));
    ImGui::InvisibleButton("timeline_canvas", ImVec2(canvasSize.x, canvasSize.y));

    // 再生ヘッドの現在位置を計算
    float currentFrameX          = canvasPos.x + drawParam_->GetHeaderWidth() + (currentFrame_ - scrollOffset_) * frameWidth;
    const float playheadHitWidth = 8.0f; // 縦線をクリックできる範囲

    // マウスクリック時：縦線の近くをクリックした場合のみドラッグ開始
    if (ImGui::IsMouseClicked(0) && ImGui::IsItemHovered()) {
        ImVec2 mousePos = ImGui::GetMousePos();
        // 縦線の近くかどうかをチェック
        if (mousePos.x >= currentFrameX - playheadHitWidth && mousePos.x <= currentFrameX + playheadHitWidth) {
            isDraggingPlayHead_ = true;
        }
    }

    // ドラッグ中：縦線を移動
    if (isDraggingPlayHead_ && ImGui::IsMouseDown(0)) {
        ImVec2 mousePos = ImGui::GetMousePos();
        if (mousePos.x > canvasPos.x + drawParam_->GetHeaderWidth()) {
            int clickedFrame = scrollOffset_ + static_cast<int>((mousePos.x - canvasPos.x - drawParam_->GetHeaderWidth()) / frameWidth);
            if (clickedFrame >= 0 && clickedFrame <= drawParam_->GetEndFrame()) {
                currentFrame_ = clickedFrame;
                applyCurrentFrameCallback();
            }
        }
    }

    // マウスリリース時：ドラッグ終了
    if (ImGui::IsMouseReleased(0)) {
        isDraggingPlayHead_ = false;
    }

    // スクロール
    if (ImGui::IsItemHovered()) {
        float wheel = ImGui::GetIO().MouseWheel;
        if (wheel != 0) {
            scrollOffset_ -= static_cast<int>(wheel * 3);
            if (scrollOffset_ < 0) {
                scrollOffset_ = 0;
            }
            if (scrollOffset_ > drawParam_->GetEndFrame() - 10) {
                scrollOffset_ = drawParam_->GetEndFrame() - 10;
            }
        }
    }
}

void TimelineInteractionHandler::UpdatePlayback(const std::function<void()>& applyCurrentFrameCallback) {
    if (isPlaying_) {
        currentFrame_++;
        if (currentFrame_ > drawParam_->GetEndFrame()) {
            currentFrame_ = 0;
        }
        applyCurrentFrameCallback();
    }
}
