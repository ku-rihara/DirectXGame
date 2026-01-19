#include "TimeLine.h"

using namespace KetaEngine;
#include <algorithm>
#include <cstdio>
#include <imgui.h>

void TimeLine::Init() {
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

uint32_t TimeLine::AddTrack(const std::string& trackName, std::function<void(float)> callback) {
    TimeLineTrack newTrack;
    newTrack.name           = trackName;
    newTrack.onValueChanged = callback;
    newTrack.id             = nextTrackId_++;

    tracks_.push_back(newTrack);
    return static_cast<uint32_t>(tracks_.size() - 1);
}

bool TimeLine::RemoveTrack(uint32_t trackIndex) {
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

void TimeLine::AddKeyFrame(uint32_t trackIndex, int32_t frame, float value, float duration, const std::string& label) {
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

bool TimeLine::RemoveKeyFrame(uint32_t trackIndex, uint32_t keyIndex) {
    if (trackIndex >= tracks_.size()) {
        return false;
    }

    if (keyIndex >= tracks_[trackIndex].keyframes.size()) {
        return false;
    }

    tracks_[trackIndex].keyframes.erase(tracks_[trackIndex].keyframes.begin() + keyIndex);
    return true;
}

bool TimeLine::SetKeyFrameLabel(uint32_t trackIndex, uint32_t keyIndex, const std::string& label) {
    if (trackIndex >= tracks_.size()) {
        return false;
    }

    if (keyIndex >= tracks_[trackIndex].keyframes.size()) {
        return false;
    }

    tracks_[trackIndex].keyframes[keyIndex].label = label;
    return true;
}

void TimeLine::SetKeyFrameDragConstraintCallback(uint32_t trackIndex,
    std::function<int32_t(int32_t, int32_t, int32_t)> callback) {
    if (trackIndex >= tracks_.size()) {
        return;
    }

    tracks_[trackIndex].onKeyFrameDragConstraint = callback;
}

float TimeLine::GetValueAtFrame(uint32_t trackIndex, int32_t frame) const {
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

int32_t TimeLine::GetFirstKeyFrameFrame(uint32_t trackIndex) const {
    if (trackIndex >= tracks_.size()) {
        return 0;
    }

    const auto& keyframes = tracks_[trackIndex].keyframes;
    if (keyframes.empty()) {
        return 0;
    }

    return keyframes.front().frame;
}

float TimeLine::InterpolateValue(const TimeLineKeyFrame& key1, const TimeLineKeyFrame& key2, int32_t frame) const {
    float t = static_cast<float>(frame - key1.frame) / static_cast<float>(key2.frame - key1.frame);
    t       = std::clamp(t, 0.0f, 1.0f);
    return key1.value + (key2.value - key1.value) * t;
}

void TimeLine::ApplyCurrentFrame() {
    for (uint32_t i = 0; i < tracks_.size(); i++) {
        if (tracks_[i].onValueChanged) {
            float value = GetValueAtFrame(i, currentFrame_);
            tracks_[i].onValueChanged(value);
        }
    }
}

void TimeLine::SetTrackRightClickCallback(uint32_t trackIndex, std::function<void(int32_t)> callback) {
    if (trackIndex >= tracks_.size()) {
        return;
    }

    tracks_[trackIndex].onRightClick = callback;
}

void TimeLine::SetKeyFrameRightClickCallback(uint32_t trackIndex, std::function<void(int32_t, int32_t)> callback) {
    if (trackIndex >= tracks_.size()) {
        return;
    }

    tracks_[trackIndex].onKeyFrameRightClick = callback;
}

void TimeLine::HandleKeyFrameDragDrop(uint32_t trackIndex, uint32_t keyIndex, const Vector2& keyPos) {
    if (ImGui::IsMouseClicked(0) && ImGui::IsMouseHoveringRect(ImVec2(keyPos.x - 8, keyPos.y - 8), ImVec2(keyPos.x + 8, keyPos.y + 8))) {

        draggingTrackIndex_                                = trackIndex;
        draggingKeyIndex_                                  = keyIndex;
        dragStartFrame_                                    = tracks_[trackIndex].keyframes[keyIndex].frame;
        tracks_[trackIndex].keyframes[keyIndex].isSelected = true;
    }
}

void TimeLine::HandleDurationDrag(uint32_t trackIndex, uint32_t keyIndex, float durationBarX, float trackY, float keyFrameX) {
    const float barHeight     = trackHeight_ * 0.8f;
    const float barTopY       = trackY + (trackHeight_ - barHeight) / 2.0f;
    const float dragZoneWidth = 8.0f;

    // durationが0または小さい場合は、キーフレーム位置から右側にドラッグゾーンを配置
    float effectiveDurationBarX = durationBarX;
    if (tracks_[trackIndex].keyframes[keyIndex].duration < 1.0f) {
        effectiveDurationBarX = keyFrameX + 20.0f; // キーフレームから20px右に配置
    }

    ImVec2 dragZoneMin = ImVec2(effectiveDurationBarX - dragZoneWidth, barTopY);
    ImVec2 dragZoneMax = ImVec2(effectiveDurationBarX + dragZoneWidth, barTopY + barHeight);

    if (ImGui::IsMouseClicked(0) && ImGui::IsMouseHoveringRect(dragZoneMin, dragZoneMax)) {
        draggingDurationTrackIndex_ = trackIndex;
        draggingDurationKeyIndex_   = keyIndex;
        dragStartDuration_          = tracks_[trackIndex].keyframes[keyIndex].duration;
    }
}

void TimeLine::HandleTrackRightClick(uint32_t trackIndex, float trackY) {
    ImVec2 mousePos   = ImGui::GetMousePos();
    ImVec2 canvas_pos = ImGui::GetCursorScreenPos();

    ImVec2 trackStart = ImVec2(canvas_pos.x, trackY);
    ImVec2 trackEnd   = ImVec2(canvas_pos.x + ImGui::GetContentRegionAvail().x, trackY + trackHeight_);

    if (ImGui::IsMouseClicked(1) && mousePos.x >= trackStart.x && mousePos.x <= trackEnd.x && mousePos.y >= trackStart.y && mousePos.y <= trackEnd.y) {

        rightClickedTrackIndex_ = trackIndex;

        if (tracks_[trackIndex].onRightClick) {
            tracks_[trackIndex].onRightClick(trackIndex);
        }

        ImGui::OpenPopup(("TrackContextMenu_" + std::to_string(trackIndex)).c_str());
    }
}

void TimeLine::Draw(const std::string& name) {

    ImGui::Begin(name.c_str(), nullptr, ImGuiWindowFlags_NoScrollbar);

    if (originalItemDrawCallBack_) {
        ImGui::SameLine();
        ImGui::Separator();
        originalItemDrawCallBack_();
    }

    // ツールバー
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

    // 描画領域取得
    ImVec2 canvas_pos     = ImGui::GetCursorScreenPos();
    ImVec2 canvas_size    = ImGui::GetContentRegionAvail();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    // 背景
    draw_list->AddRectFilled(canvas_pos,
        ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y),
        IM_COL32(30, 30, 30, 255));

    const float frameWidth = 10.0f * zoom_;

    // ルーラー描画
    ImVec2 ruler_start = canvas_pos;
    ImVec2 ruler_end   = ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + rulerHeight_);
    draw_list->AddRectFilled(ruler_start, ruler_end, IM_COL32(50, 50, 50, 255));

    // フレーム番号とグリッド
    int visibleFrameStart = scrollOffset_;
    int visibleFrameEnd   = scrollOffset_ + static_cast<int>((canvas_size.x - headerWidth_) / frameWidth) + 1;

    float trackAreaHeight = rulerHeight_ + tracks_.size() * trackHeight_;

    for (int frame = visibleFrameStart; frame <= visibleFrameEnd && frame <= endFrame_; frame++) {
        float x = canvas_pos.x + headerWidth_ + (frame - scrollOffset_) * frameWidth;

        if (frame % 5 == 0) {
            draw_list->AddLine(
                ImVec2(x, canvas_pos.y + rulerHeight_),
                ImVec2(x, canvas_pos.y + trackAreaHeight),
                IM_COL32(60, 60, 60, 255), 1.0f);

            char frameText[8];
            std::snprintf(frameText, sizeof(frameText), "%d", frame);
            draw_list->AddText(ImVec2(x + 2, canvas_pos.y + 5), IM_COL32(200, 200, 200, 255), frameText);
        } else {
            draw_list->AddLine(
                ImVec2(x, canvas_pos.y + rulerHeight_),
                ImVec2(x, canvas_pos.y + trackAreaHeight),
                IM_COL32(45, 45, 45, 255), 1.0f);
        }
    }

    // トラック描画
    float trackY = canvas_pos.y + rulerHeight_;

    for (uint32_t i = 0; i < tracks_.size(); i++) {
        TimeLineTrack& track = tracks_[i];

        // トラックヘッダー
        ImVec2 headerStart = ImVec2(canvas_pos.x, trackY);
        ImVec2 headerEnd   = ImVec2(canvas_pos.x + headerWidth_, trackY + trackHeight_);
        draw_list->AddRectFilled(headerStart, headerEnd, IM_COL32(40, 40, 40, 255));
        draw_list->AddRect(headerStart, headerEnd, IM_COL32(70, 70, 70, 255));
        draw_list->AddText(ImVec2(canvas_pos.x + 10, trackY + 8),
            IM_COL32(255, 255, 255, 255), track.name.c_str());

        // トラックライン背景
        ImVec2 lineStart = ImVec2(canvas_pos.x + headerWidth_, trackY);
        ImVec2 lineEnd   = ImVec2(canvas_pos.x + canvas_size.x, trackY + trackHeight_);
        draw_list->AddRectFilled(lineStart, lineEnd,
            (i % 2 == 0) ? IM_COL32(35, 35, 35, 255) : IM_COL32(30, 30, 30, 255));
        draw_list->AddLine(ImVec2(canvas_pos.x, trackY + trackHeight_),
            ImVec2(canvas_pos.x + canvas_size.x, trackY + trackHeight_),
            IM_COL32(50, 50, 50, 255));

        // 右クリック処理
        HandleTrackRightClick(i, trackY);

        // キーフレーム描画
        for (uint32_t j = 0; j < track.keyframes.size(); j++) {
            TimeLineKeyFrame& kf = track.keyframes[j];

            if (kf.frame >= visibleFrameStart && kf.frame <= visibleFrameEnd) {
                float kfX = canvas_pos.x + headerWidth_ + (kf.frame - scrollOffset_) * frameWidth;
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
                if (kf.duration > 0.0f) {
                    float durationWidth = std::max(kf.duration * frameWidth, 1.0f);
                    float barHeight     = trackHeight_ * 0.8f;
                    float barTopY       = trackY + (trackHeight_ - barHeight) / 2.0f;

                    draw_list->AddRectFilled(
                        ImVec2(kfX, barTopY),
                        ImVec2(kfX + durationWidth, barTopY + barHeight),
                        durationUIColor);

                    // ラベル表示
                    if (!kf.label.empty()) {
                        draw_list->AddText(
                            ImVec2(kfX + 5, barTopY + 2),
                            IM_COL32(255, 255, 255, 255),
                            kf.label.c_str());
                    }

                    // duration ドラッグハンドル処理と視覚的表示
                    float durationEndX = kfX + durationWidth;

                    // リサイズハンドルを描画（縦線3本）
                    float handleCenterX = durationEndX;
                    draw_list->AddLine(
                        ImVec2(handleCenterX - 3, barTopY + 4),
                        ImVec2(handleCenterX - 3, barTopY + barHeight - 4),
                        IM_COL32(200, 200, 200, 200), 1.5f);
                    draw_list->AddLine(
                        ImVec2(handleCenterX, barTopY + 4),
                        ImVec2(handleCenterX, barTopY + barHeight - 4),
                        IM_COL32(255, 255, 255, 255), 2.0f);
                    draw_list->AddLine(
                        ImVec2(handleCenterX + 3, barTopY + 4),
                        ImVec2(handleCenterX + 3, barTopY + barHeight - 4),
                        IM_COL32(200, 200, 200, 200), 1.5f);

                    HandleDurationDrag(i, j, durationEndX, trackY, kfX);
                } else {
                    // durationが0の場合でもドラッグハンドルを表示
                    float barHeight = trackHeight_ * 0.8f;
                    float barTopY   = trackY + (trackHeight_ - barHeight) / 2.0f;
                    float handleX   = kfX + 20.0f; // キーフレームから20px右

                    // リサイズハンドルを描画（縦線3本）
                    draw_list->AddLine(
                        ImVec2(handleX - 3, barTopY + 4),
                        ImVec2(handleX - 3, barTopY + barHeight - 4),
                        IM_COL32(150, 150, 150, 150), 1.5f);
                    draw_list->AddLine(
                        ImVec2(handleX, barTopY + 4),
                        ImVec2(handleX, barTopY + barHeight - 4),
                        IM_COL32(180, 180, 180, 180), 2.0f);
                    draw_list->AddLine(
                        ImVec2(handleX + 3, barTopY + 4),
                        ImVec2(handleX + 3, barTopY + barHeight - 4),
                        IM_COL32(150, 150, 150, 150), 1.5f);

                    // ドラッグハンドル処理
                    HandleDurationDrag(i, j, handleX, trackY, kfX);
                }

                // キーフレームマーカー(ダイヤモンド)
                ImVec2 diamond[4] = {
                    ImVec2(kfX, kfY - 6),
                    ImVec2(kfX + 6, kfY),
                    ImVec2(kfX, kfY + 6),
                    ImVec2(kfX - 6, kfY)};

                draw_list->AddConvexPolyFilled(diamond, 4, color);

                // ホバー時のラベル表示
                if (!kf.label.empty() && ImGui::IsMouseHoveringRect(ImVec2(kfX - 8, kfY - 8), ImVec2(kfX + 8, kfY + 8))) {
                    ImGui::SetTooltip("%s", kf.label.c_str());
                }

                // ドラッグ&ドロップ処理
                HandleKeyFrameDragDrop(i, j, Vector2(kfX, kfY));

                // キーフレーム右クリック
                if (ImGui::IsMouseClicked(1) && ImGui::IsMouseHoveringRect(ImVec2(kfX - 8, kfY - 8), ImVec2(kfX + 8, kfY + 8))) {
                    ImGui::OpenPopup(("KeyFrameContextMenu_" + std::to_string(i) + "_" + std::to_string(j)).c_str());
                }

                // キーフレームコンテキストメニュー
                if (ImGui::BeginPopup(("KeyFrameContextMenu_" + std::to_string(i) + "_" + std::to_string(j)).c_str())) {
                    // カスタムコールバックがある場合は、それを呼び出して描画
                    if (track.onKeyFrameRightClick) {
                        track.onKeyFrameRightClick(i, j);
                    }

                    // 削除メニュー
                    if (ImGui::MenuItem("Delete KeyFrame")) {
                        RemoveKeyFrame(i, j);
                    }
                    ImGui::EndPopup();
                }
            }
        }

        trackY += trackHeight_;
    }

    // ドラッグ中の処理(キーフレーム位置)
    if (draggingTrackIndex_ >= 0 && draggingKeyIndex_ >= 0 && ImGui::IsMouseDragging(0)) {
        ImVec2 mouse_pos = ImGui::GetMousePos();
        float deltaX     = mouse_pos.x - (canvas_pos.x + headerWidth_ + (dragStartFrame_ - scrollOffset_) * frameWidth);
        int deltaFrame   = static_cast<int>(deltaX / frameWidth);
        int newFrame     = dragStartFrame_ + deltaFrame;

        if (newFrame >= 0 && newFrame <= endFrame_) {
            // 制約コールバックがある場合は適用
            if (tracks_[draggingTrackIndex_].onKeyFrameDragConstraint) {
                newFrame = tracks_[draggingTrackIndex_].onKeyFrameDragConstraint(
                    draggingTrackIndex_, draggingKeyIndex_, newFrame);
            }

            tracks_[draggingTrackIndex_].keyframes[draggingKeyIndex_].frame = newFrame;
        }
    }

    // ドラッグ中の処理(duration)
    if (draggingDurationTrackIndex_ >= 0 && draggingDurationKeyIndex_ >= 0 && ImGui::IsMouseDragging(0)) {
        // ドラッグ中もカーソルを変更
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);

        ImVec2 mouse_pos  = ImGui::GetMousePos();
        auto& kf          = tracks_[draggingDurationTrackIndex_].keyframes[draggingDurationKeyIndex_];
        float kfX         = canvas_pos.x + headerWidth_ + (kf.frame - scrollOffset_) * frameWidth;
        float deltaX      = mouse_pos.x - kfX;
        float newDuration = deltaX / frameWidth;

        // 0以上であれば設定可能（最小値制限なし）
        if (newDuration >= 0.0f) {
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

    // 終端ライン描画
    float endFrameX = canvas_pos.x + headerWidth_ + (endFrame_ - scrollOffset_) * frameWidth;
    if (endFrameX >= canvas_pos.x + headerWidth_ && endFrameX <= canvas_pos.x + canvas_size.x) {
        float dashLength = 5.0f;
        float gapLength  = 3.0f;
        float currentY   = canvas_pos.y;

        while (currentY < canvas_pos.y + trackAreaHeight) {
            float nextY = currentY + dashLength;
            if (nextY > canvas_pos.y + trackAreaHeight) {
                nextY = canvas_pos.y + trackAreaHeight;
            }

            draw_list->AddLine(
                ImVec2(endFrameX, currentY),
                ImVec2(endFrameX, nextY),
                IM_COL32(255, 50, 50, 255), 2.0f);

            currentY = nextY + gapLength;
        }
    }

    // インタラクション
    ImGui::SetCursorScreenPos(canvas_pos);
    ImGui::InvisibleButton("timeline_canvas", canvas_size);

    // マウス長押しでフレームを移動
    if (ImGui::IsItemActive() && ImGui::IsMouseDown(0)) {
        ImVec2 mouse_pos = ImGui::GetMousePos();
        if (mouse_pos.x > canvas_pos.x + headerWidth_) {
            int clickedFrame = scrollOffset_ + (int)((mouse_pos.x - canvas_pos.x - headerWidth_) / frameWidth);
            if (clickedFrame >= 0 && clickedFrame <= endFrame_) {
                currentFrame_ = clickedFrame;
                ApplyCurrentFrame();
            }
        }
    }

    // 現在フレームインジケーター
    float currentFrameX = canvas_pos.x + headerWidth_ + (currentFrame_ - scrollOffset_) * frameWidth;
    draw_list->AddLine(
        ImVec2(currentFrameX, canvas_pos.y),
        ImVec2(currentFrameX, canvas_pos.y + trackAreaHeight),
        IM_COL32(255, 100, 100, 255), 2.0f);

    // スクロール
    if (ImGui::IsItemHovered()) {
        float wheel = ImGui::GetIO().MouseWheel;
        if (wheel != 0) {
            scrollOffset_ -= (int)(wheel * 3);
            if (scrollOffset_ < 0)
                scrollOffset_ = 0;
            if (scrollOffset_ > endFrame_ - 10)
                scrollOffset_ = endFrame_ - 10;
        }
    }

    // 再生中の処理
    if (isPlaying_) {
        currentFrame_++;
        if (currentFrame_ > endFrame_) {
            currentFrame_ = 0;
        }
        ApplyCurrentFrame();
    }

    ImGui::End();
}