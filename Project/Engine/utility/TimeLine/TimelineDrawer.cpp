#include "TimelineDrawer.h"

using namespace KetaEngine;
#include <algorithm>
#include <cstdio>


void TimelineDrawer::Init() {
    tracks_.clear();
    currentFrame_               = 0;
    scrollOffset_               = 0;
    isPlaying_                  = false;
    draggingTrackIndex_         = -1;
    draggingKeyIndex_           = -1;
    draggingDurationTrackIndex_ = -1;
    draggingDurationKeyIndex_   = -1;
    rightClickedTrackIndex_     = -1;
    nextTrackId_                = 0;
}

uint32_t TimelineDrawer::AddTrack(const std::string& trackName, std::function<void(float)> callback) {
    TimeLineTrack newTrack;
    newTrack.name           = trackName;
    newTrack.onValueChanged = callback;
    newTrack.id             = nextTrackId_++;

    tracks_.push_back(newTrack);
    return static_cast<uint32_t>(tracks_.size() - 1);
}

uint32_t TimelineDrawer::InsertTrack(uint32_t position, const std::string& trackName, std::function<void(float)> callback) {
    TimeLineTrack newTrack;
    newTrack.name           = trackName;
    newTrack.onValueChanged = callback;
    newTrack.id             = nextTrackId_++;

    // 位置が範囲外の場合は末尾に追加
    if (position >= tracks_.size()) {
        tracks_.push_back(newTrack);
        return static_cast<uint32_t>(tracks_.size() - 1);
    }

    // 指定位置に挿入
    tracks_.insert(tracks_.begin() + position, newTrack);
    return position;
}

bool TimelineDrawer::RemoveTrack(uint32_t trackIndex) {
    if (trackIndex >= tracks_.size()) {
        return false;
    }

    tracks_.erase(tracks_.begin() + trackIndex);

    if (draggingTrackIndex_ == static_cast<int>(trackIndex)) {
        draggingTrackIndex_ = -1;
        draggingKeyIndex_   = -1;
    } else if (draggingTrackIndex_ > static_cast<int>(trackIndex)) {
        draggingTrackIndex_--;
    }

    if (draggingDurationTrackIndex_ == static_cast<int>(trackIndex)) {
        draggingDurationTrackIndex_ = -1;
        draggingDurationKeyIndex_   = -1;
    } else if (draggingDurationTrackIndex_ > static_cast<int>(trackIndex)) {
        draggingDurationTrackIndex_--;
    }

    if (rightClickedTrackIndex_ == static_cast<int>(trackIndex)) {
        rightClickedTrackIndex_ = -1;
    } else if (rightClickedTrackIndex_ > static_cast<int>(trackIndex)) {
        rightClickedTrackIndex_--;
    }

    return true;
}

void TimelineDrawer::AddKeyFrame(uint32_t trackIndex, int32_t frame, float value, float duration, const std::string& label) {
    if (trackIndex >= tracks_.size()) {
        return;
    }

    TimeLineKeyFrame newKeyFrame;
    newKeyFrame.frame      = frame;
    newKeyFrame.value      = value;
    newKeyFrame.duration   = duration;
    newKeyFrame.isSelected = false;
    newKeyFrame.label      = label;

    tracks_[trackIndex].keyframes.push_back(newKeyFrame);

    std::sort(tracks_[trackIndex].keyframes.begin(), tracks_[trackIndex].keyframes.end(),
        [](const TimeLineKeyFrame& a, const TimeLineKeyFrame& b) { return a.frame < b.frame; });
}

bool TimelineDrawer::RemoveKeyFrame(uint32_t trackIndex, uint32_t keyIndex) {
    if (trackIndex >= tracks_.size()) {
        return false;
    }

    if (keyIndex >= tracks_[trackIndex].keyframes.size()) {
        return false;
    }

    tracks_[trackIndex].keyframes.erase(tracks_[trackIndex].keyframes.begin() + keyIndex);
    return true;
}

bool TimelineDrawer::SetKeyFrameLabel(uint32_t trackIndex, uint32_t keyIndex, const std::string& label) {
    if (trackIndex >= tracks_.size()) {
        return false;
    }

    if (keyIndex >= tracks_[trackIndex].keyframes.size()) {
        return false;
    }

    tracks_[trackIndex].keyframes[keyIndex].label = label;
    return true;
}

float TimelineDrawer::GetValueAtFrame(uint32_t trackIndex, int32_t frame) const {
    if (trackIndex >= tracks_.size()) {
        return 0.0f;
    }

    const auto& keyframes = tracks_[trackIndex].keyframes;
    if (keyframes.empty()) {
        return 0.0f;
    }

    if (frame <= keyframes.front().frame) {
        return keyframes.front().value;
    }

    for (size_t i = 0; i < keyframes.size() - 1; ++i) {
        if (frame >= keyframes[i].frame && frame <= keyframes[i + 1].frame) {
            return InterpolateValue(keyframes[i], keyframes[i + 1], frame);
        }
    }

    return keyframes.back().value;
}

int32_t TimelineDrawer::GetFirstKeyFrameFrame(uint32_t trackIndex) const {
    if (trackIndex >= tracks_.size()) {
        return 0;
    }

    const auto& keyframes = tracks_[trackIndex].keyframes;
    if (keyframes.empty()) {
        return 0;
    }

    return keyframes.front().frame;
}

float TimelineDrawer::InterpolateValue(const TimeLineKeyFrame& key1, const TimeLineKeyFrame& key2, int32_t frame) const {
    float t = static_cast<float>(frame - key1.frame) / static_cast<float>(key2.frame - key1.frame);
    t       = std::clamp(t, 0.0f, 1.0f);
    return key1.value + (key2.value - key1.value) * t;
}

void TimelineDrawer::ApplyCurrentFrame() {
    for (uint32_t i = 0; i < tracks_.size(); i++) {
        if (tracks_[i].onValueChanged) {
            float value = GetValueAtFrame(i, currentFrame_);
            tracks_[i].onValueChanged(value);
        }
    }
}

void TimelineDrawer::SetKeyFrameRightClickCallback(uint32_t trackIndex, std::function<void(int32_t, int32_t)> callback) {
    if (trackIndex >= tracks_.size()) {
        return;
    }

    tracks_[trackIndex].onKeyFrameRightClick = callback;
}

void TimelineDrawer::HandleKeyFrameDragDrop(uint32_t trackIndex, uint32_t keyIndex, const Vector2& keyPos) {
    if (ImGui::IsMouseClicked(0) && ImGui::IsMouseHoveringRect(ImVec2(keyPos.x - 8, keyPos.y - 8), ImVec2(keyPos.x + 8, keyPos.y + 8))) {

        draggingTrackIndex_                                = trackIndex;
        draggingKeyIndex_                                  = keyIndex;
        dragStartFrame_                                    = tracks_[trackIndex].keyframes[keyIndex].frame;
        tracks_[trackIndex].keyframes[keyIndex].isSelected = true;
    }
}

void TimelineDrawer::HandleDurationDrag(uint32_t trackIndex, uint32_t keyIndex, float durationBarX, float trackY) {
    const float barHeight     = trackHeight_ * 0.8f;
    const float barTopY       = trackY + (trackHeight_ - barHeight) / 2.0f;
    const float dragZoneWidth = 8.0f;

    ImVec2 dragZoneMin = ImVec2(durationBarX - dragZoneWidth, barTopY);
    ImVec2 dragZoneMax = ImVec2(durationBarX + dragZoneWidth, barTopY + barHeight);

    if (ImGui::IsMouseClicked(0) && ImGui::IsMouseHoveringRect(dragZoneMin, dragZoneMax)) {
        draggingDurationTrackIndex_ = trackIndex;
        draggingDurationKeyIndex_   = keyIndex;
        dragStartDuration_          = tracks_[trackIndex].keyframes[keyIndex].duration;
    }
}

void TimelineDrawer::Draw(const std::string& name) {
    ImGui::Begin(name.c_str(), nullptr, ImGuiWindowFlags_NoScrollbar);

    // オリジナルのUIを描画
    if (originalItemDrawCallBack_) {
        ImGui::SameLine();
        ImGui::Separator();
        originalItemDrawCallBack_();
    }

    DrawToolbar();

    // 描画領域取得
    ImVec2 canvasPosImGui  = ImGui::GetCursorScreenPos();
    ImVec2 canvasSizeImGui = ImGui::GetContentRegionAvail();
    Vector2 canvasPos      = Vector2(canvasPosImGui.x, canvasPosImGui.y);
    Vector2 canvasSize     = Vector2(canvasSizeImGui.x, canvasSizeImGui.y);
    ImDrawList* drawList   = ImGui::GetWindowDrawList();

    const float frameWidth    = 10.0f * zoom_;
    int visibleFrameStart     = scrollOffset_;
    int visibleFrameEnd       = scrollOffset_ + static_cast<int>((canvasSize.x - headerWidth_) / frameWidth) + 1;
    float trackAreaHeight     = rulerHeight_ + tracks_.size() * trackHeight_;

    // 背景描画
    DrawBackground(drawList, canvasPos, canvasSize);
    // ルーラーとグリッドの描画
    DrawRulerAndGrid(drawList, canvasPos, canvasSize, frameWidth, trackAreaHeight, visibleFrameStart, visibleFrameEnd);
    // トラックの描画
    DrawTracks(drawList, canvasPos, canvasSize, frameWidth, visibleFrameStart, visibleFrameEnd);
    // ドラッグ処理の更新
    HandleDragUpdates(canvasPos, frameWidth);
    // フレームの終点の点線描画
    DrawEndFrameLine(drawList, canvasPos, frameWidth, trackAreaHeight);
    // キャンバスのインタラクション
    HandleCanvasInteraction(canvasPos, canvasSize, frameWidth, trackAreaHeight);

    // 現在フレームインジケーター
    float currentFrameX = canvasPos.x + headerWidth_ + (currentFrame_ - scrollOffset_) * frameWidth;
    drawList->AddLine(
        ImVec2(currentFrameX, canvasPos.y),
        ImVec2(currentFrameX, canvasPos.y + trackAreaHeight),
        IM_COL32(255, 100, 100, 255), 2.0f);

    UpdatePlayback();

    ImGui::End();
}

void TimelineDrawer::DrawToolbar() {
    ImGui::Text("Frame: %d / %d", currentFrame_, endFrame_);
    ImGui::SameLine();

    if (ImGui::Button(isPlaying_ ? "Pause" : "Play")) {
        isPlaying_ = !isPlaying_;
    }

    ImGui::SameLine();

    if (ImGui::Button("Stop")) {
        currentFrame_ = 0;
        isPlaying_    = false;
    }
    ImGui::SameLine();

    ImGui::SetNextItemWidth(100);
    ImGui::SliderFloat("Zoom", &zoom_, 0.1f, 5.0f);

    ImGui::Separator();
}

void TimelineDrawer::DrawBackground(::ImDrawList* drawList, const Vector2& canvasPos, const Vector2& canvasSize) {
    drawList->AddRectFilled(
        ImVec2(canvasPos.x, canvasPos.y),
        ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y),
        IM_COL32(30, 30, 30, 255));
}

void TimelineDrawer::DrawRulerAndGrid(::ImDrawList* drawList, const Vector2& canvasPos,
    const Vector2& canvasSize, float frameWidth, float trackAreaHeight,
    int visibleFrameStart, int visibleFrameEnd) {

    // ルーラー背景
    ImVec2 rulerStart = ImVec2(canvasPos.x, canvasPos.y);
    ImVec2 rulerEnd   = ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + rulerHeight_);
    drawList->AddRectFilled(rulerStart, rulerEnd, IM_COL32(50, 50, 50, 255));

    // フレーム番号とグリッド
    for (int frame = visibleFrameStart; frame <= visibleFrameEnd && frame <= endFrame_; frame++) {
        float x = canvasPos.x + headerWidth_ + (frame - scrollOffset_) * frameWidth;

        if (frame % 5 == 0) {
            drawList->AddLine(
                ImVec2(x, canvasPos.y + rulerHeight_),
                ImVec2(x, canvasPos.y + trackAreaHeight),
                IM_COL32(60, 60, 60, 255), 1.0f);

            char frameText[8];
            std::snprintf(frameText, sizeof(frameText), "%d", frame);
            drawList->AddText(ImVec2(x + 2, canvasPos.y + 5), IM_COL32(200, 200, 200, 255), frameText);
        } else {
            drawList->AddLine(
                ImVec2(x, canvasPos.y + rulerHeight_),
                ImVec2(x, canvasPos.y + trackAreaHeight),
                IM_COL32(45, 45, 45, 255), 1.0f);
        }
    }
}

void TimelineDrawer::DrawTracks(::ImDrawList* drawList, const Vector2& canvasPos,
    const Vector2& canvasSize, float frameWidth,
    int visibleFrameStart, int visibleFrameEnd) {

    float trackY = canvasPos.y + rulerHeight_;

    for (uint32_t i = 0; i < tracks_.size(); i++) {
        TimeLineTrack& track = tracks_[i];

        // トラックヘッダー
        ImVec2 headerStart = ImVec2(canvasPos.x, trackY);
        ImVec2 headerEnd   = ImVec2(canvasPos.x + headerWidth_, trackY + trackHeight_);
        drawList->AddRectFilled(headerStart, headerEnd, IM_COL32(40, 40, 40, 255));
        drawList->AddRect(headerStart, headerEnd, IM_COL32(70, 70, 70, 255));
        drawList->AddText(ImVec2(canvasPos.x + 10, trackY + 8),
            IM_COL32(255, 255, 255, 255), track.name.c_str());

        // トラックライン背景
        ImVec2 lineStart = ImVec2(canvasPos.x + headerWidth_, trackY);
        ImVec2 lineEnd   = ImVec2(canvasPos.x + canvasSize.x, trackY + trackHeight_);
        drawList->AddRectFilled(lineStart, lineEnd,
            (i % 2 == 0) ? IM_COL32(35, 35, 35, 255) : IM_COL32(30, 30, 30, 255));
        drawList->AddLine(ImVec2(canvasPos.x, trackY + trackHeight_),
            ImVec2(canvasPos.x + canvasSize.x, trackY + trackHeight_),
            IM_COL32(50, 50, 50, 255));

        // キーフレーム描画
        for (uint32_t j = 0; j < track.keyframes.size(); j++) {
            DrawKeyFrame(drawList, i, j, canvasPos, frameWidth, visibleFrameStart, visibleFrameEnd, trackY);
        }

        trackY += trackHeight_;
    }
}

void TimelineDrawer::DrawKeyFrame(::ImDrawList* drawList, uint32_t trackIndex, uint32_t keyIndex,
    const Vector2& canvasPos, float frameWidth,
    int visibleFrameStart, int visibleFrameEnd, float trackY) {

    TimeLineTrack& track = tracks_[trackIndex];
    TimeLineKeyFrame& kf = track.keyframes[keyIndex];

    if (kf.frame < visibleFrameStart || kf.frame > visibleFrameEnd) {
        return;
    }

    float kfX = canvasPos.x + headerWidth_ + (kf.frame - scrollOffset_) * frameWidth;
    float kfY = trackY + trackHeight_ / 2;

    bool isOnCurrentFrame = (currentFrame_ >= kf.frame && currentFrame_ <= kf.frame + static_cast<int32_t>(kf.duration));

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
        float barHeight     = trackHeight_ * 0.8f;
        float barTopY       = trackY + (trackHeight_ - barHeight) / 2.0f;

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

        // duration ドラッグハンドル処理
        float durationEndX = kfX + durationWidth;
        HandleDurationDrag(trackIndex, keyIndex, durationEndX, trackY);
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

    // ドラッグ&ドロップ処理
    HandleKeyFrameDragDrop(trackIndex, keyIndex, Vector2(kfX, kfY));

    // キーフレーム右クリック
    if (ImGui::IsMouseClicked(1) && ImGui::IsMouseHoveringRect(ImVec2(kfX - 8, kfY - 8), ImVec2(kfX + 8, kfY + 8))) {
        ImGui::OpenPopup(("KeyFrameContextMenu_" + std::to_string(trackIndex) + "_" + std::to_string(keyIndex)).c_str());
    }

    // キーフレームコンテキストメニュー
    if (ImGui::BeginPopup(("KeyFrameContextMenu_" + std::to_string(trackIndex) + "_" + std::to_string(keyIndex)).c_str())) {
        if (ImGui::MenuItem("横棒を1フレーム伸ばす")) {
            tracks_[trackIndex].keyframes[keyIndex].duration += 1.0f;
        }

        ImGui::Separator();

        if (track.onKeyFrameRightClick) {
            track.onKeyFrameRightClick(trackIndex, keyIndex);
        }

        if (ImGui::MenuItem("Delete KeyFrame")) {
            RemoveKeyFrame(trackIndex, keyIndex);
        }
        ImGui::EndPopup();
    }
}

void TimelineDrawer::HandleDragUpdates(const Vector2& canvasPos, float frameWidth) {
    // ドラッグ中の処理(キーフレーム位置)
    if (draggingTrackIndex_ >= 0 && draggingKeyIndex_ >= 0 && ImGui::IsMouseDragging(0)) {
        ImVec2 mousePos = ImGui::GetMousePos();
        float deltaX    = mousePos.x - (canvasPos.x + headerWidth_ + (dragStartFrame_ - scrollOffset_) * frameWidth);
        int deltaFrame  = static_cast<int>(deltaX / frameWidth);
        int newFrame    = dragStartFrame_ + deltaFrame;

        if (newFrame >= 0 && newFrame <= endFrame_) {
            tracks_[draggingTrackIndex_].keyframes[draggingKeyIndex_].frame = newFrame;
        }
    }

    // ドラッグ中の処理(duration)
    if (draggingDurationTrackIndex_ >= 0 && draggingDurationKeyIndex_ >= 0 && ImGui::IsMouseDragging(0)) {
        ImVec2 mousePos   = ImGui::GetMousePos();
        auto& kf          = tracks_[draggingDurationTrackIndex_].keyframes[draggingDurationKeyIndex_];
        float kfX         = canvasPos.x + headerWidth_ + (kf.frame - scrollOffset_) * frameWidth;
        float deltaX      = mousePos.x - kfX;
        float newDuration = deltaX / frameWidth;

        if (newDuration >= 1.0f) {
            kf.duration = newDuration;
        }
    }

    // ドラッグ終了
    if (ImGui::IsMouseReleased(0)) {
        if (draggingTrackIndex_ >= 0 && draggingKeyIndex_ >= 0) {
            std::sort(tracks_[draggingTrackIndex_].keyframes.begin(),
                tracks_[draggingTrackIndex_].keyframes.end(),
                [](const TimeLineKeyFrame& a, const TimeLineKeyFrame& b) { return a.frame < b.frame; });

            tracks_[draggingTrackIndex_].keyframes[draggingKeyIndex_].isSelected = false;
        }
        draggingTrackIndex_         = -1;
        draggingKeyIndex_           = -1;
        draggingDurationTrackIndex_ = -1;
        draggingDurationKeyIndex_   = -1;
    }
}

void TimelineDrawer::DrawEndFrameLine(::ImDrawList* drawList, const Vector2& canvasPos,
    float frameWidth, float trackAreaHeight) {

    float endFrameX = canvasPos.x + headerWidth_ + (endFrame_ - scrollOffset_) * frameWidth;
    float canvasEndX = canvasPos.x + headerWidth_;

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

void TimelineDrawer::HandleCanvasInteraction(const Vector2& canvasPos, const Vector2& canvasSize,
    float frameWidth, float trackAreaHeight) {

    trackAreaHeight;
    ImGui::SetCursorScreenPos(ImVec2(canvasPos.x, canvasPos.y));
    ImGui::InvisibleButton("timeline_canvas", ImVec2(canvasSize.x, canvasSize.y));

    // マウス長押しでフレームを移動
    if (ImGui::IsItemActive() && ImGui::IsMouseDown(0)) {
        ImVec2 mousePos = ImGui::GetMousePos();
        if (mousePos.x > canvasPos.x + headerWidth_) {
            int clickedFrame = scrollOffset_ + static_cast<int>((mousePos.x - canvasPos.x - headerWidth_) / frameWidth);
            if (clickedFrame >= 0 && clickedFrame <= endFrame_) {
                currentFrame_ = clickedFrame;
                ApplyCurrentFrame();
            }
        }
    }

    // スクロール
    if (ImGui::IsItemHovered()) {
        float wheel = ImGui::GetIO().MouseWheel;
        if (wheel != 0) {
            scrollOffset_ -= static_cast<int>(wheel * 3);
            if (scrollOffset_ < 0) {
                scrollOffset_ = 0;
            }
            if (scrollOffset_ > endFrame_ - 10) {
                scrollOffset_ = endFrame_ - 10;
            }
        }
    }
}

void TimelineDrawer::UpdatePlayback() {
    if (isPlaying_) {
        currentFrame_++;
        if (currentFrame_ > endFrame_) {
            currentFrame_ = 0;
        }
        ApplyCurrentFrame();
    }
}