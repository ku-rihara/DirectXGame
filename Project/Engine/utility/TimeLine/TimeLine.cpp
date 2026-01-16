#include "TImeLine.h"

using namespace KetaEngine;
#include <algorithm>
#include <cstdio>
#include <imgui.h>

void TimeLine::Init() {
    tracks_.clear();
    currentFrame_       = 0;
    scrollOffset_       = 0;
    isPlaying_          = false;
    draggingTrackIndex_ = -1;
    draggingKeyIndex_   = -1;
}

uint32_t TimeLine::AddTrack(const std::string& trackName, std::function<void(float)> callback) {
    TimeLineTrack newTrack;

    // トラック情報設定
    newTrack.name           = trackName;
    newTrack.onValueChanged = callback;

    // tracksに追加
    tracks_.push_back(newTrack);
    return static_cast<uint32_t>(tracks_.size() - 1);
}

void TimeLine::AddKeyFrame(uint32_t trackIndex, int32_t frame, float value, float duration) {

    // 範囲外チェック
    if (trackIndex >= tracks_.size()) {
        return;
    }

    // キーフレーム作成
    TimeLineKeyFrame newKeyFrame;
    newKeyFrame.frame    = frame;
    newKeyFrame.value    = value;
    newKeyFrame.duration = duration;

    // tracks_.keyFramesに追加
    tracks_[trackIndex].keyframes.push_back(newKeyFrame);

    // フレーム順にソート
    std::sort(tracks_[trackIndex].keyframes.begin(), tracks_[trackIndex].keyframes.end(),
        [](const TimeLineKeyFrame& a, const TimeLineKeyFrame& b) { return a.frame < b.frame; });
}

float TimeLine::GetValueAtFrame(uint32_t trackIndex, int32_t frame) const {

    // 範囲外チェック
    if (trackIndex >= tracks_.size()) {
        return 0.0f;
    }

    // キーフレームが存在しない場合は0を返す
    const auto& keyframes = tracks_[trackIndex].keyframes;
    if (keyframes.empty()) {
        return 0.0f;
    }

    // フレームがキーフレームより前の場合
    if (frame <= keyframes.front().frame) {
        return keyframes.front().value;
    }

    // 2つのキーフレーム間で補間
    for (size_t i = 0; i < keyframes.size() - 1; ++i) { 
        if (frame >= keyframes[i].frame && frame <= keyframes[i + 1].frame) {
            return InterpolateValue(keyframes[i], keyframes[i + 1], frame);
        }
    }

    return 0.0f;
}

float TimeLine::InterpolateValue(const TimeLineKeyFrame& key1, const TimeLineKeyFrame& key2, int frame) const {
    float t = static_cast<float>(frame - key1.frame) / static_cast<float>(key2.frame - key1.frame);
    t       = std::clamp(t, 0.0f, 1.0f);
    return key1.value + (key2.value - key1.value) * t; // 線形補間
}

void TimeLine::ApplyCurrentFrame() {
    for (uint32_t i = 0; i < tracks_.size(); i++) {
        if (tracks_[i].onValueChanged) {
            float value = GetValueAtFrame(i, currentFrame_);
            tracks_[i].onValueChanged(value);
        }
    }
}

void TimeLine::HandleKeyFrameDragDrop(uint32_t trackIndex, uint32_t keyIndex, const Vector2& keyPos) {
    // ドラッグ開始
    if (ImGui::IsMouseClicked(0) && ImGui::IsMouseHoveringRect(ImVec2(keyPos.x - 8, keyPos.y - 8), ImVec2(keyPos.x + 8, keyPos.y + 8))) {

        draggingTrackIndex_                                = trackIndex;
        draggingKeyIndex_                                  = keyIndex;
        dragStartFrame_                                    = tracks_[trackIndex].keyframes[keyIndex].frame;
        tracks_[trackIndex].keyframes[keyIndex].isSelected = true;
    }
}

void TimeLine::Draw() {

    ImGui::Begin("TimeLine Editor", nullptr, ImGuiWindowFlags_NoScrollbar);

    // ツールバー
    ImGui::Text("Frame: %d / %d", currentFrame_, endFrame_);
    ImGui::SameLine();

    //
    if (ImGui::Button(isPlaying_ ? "Pause" : "Play")) {
        isPlaying_ = !isPlaying_;
    }

    ImGui::SameLine();

    if (ImGui::Button("Stop")) {
        currentFrame_ = 0;
        isPlaying_    = false;
    }
    ImGui::SameLine();

    //
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

    for (int frame = visibleFrameStart; frame <= visibleFrameEnd && frame <= endFrame_; frame++) {
        float x = canvas_pos.x + headerWidth_ + (frame - scrollOffset_) * frameWidth;

        if (frame % 5 == 0) {
            draw_list->AddLine(
                ImVec2(x, canvas_pos.y + rulerHeight_),
                ImVec2(x, canvas_pos.y + canvas_size.y),
                IM_COL32(60, 60, 60, 255), 1.0f);

            char frameText[8];
            std::snprintf(frameText, sizeof(frameText), "%d", frame);
            draw_list->AddText(ImVec2(x + 2, canvas_pos.y + 5), IM_COL32(200, 200, 200, 255), frameText);
        } else {
            draw_list->AddLine(
                ImVec2(x, canvas_pos.y + rulerHeight_),
                ImVec2(x, canvas_pos.y + canvas_size.y),
                IM_COL32(45, 45, 45, 255), 1.0f);
        }
    }

     // 現在フレームインジケーター
    float currentFrameX = canvas_pos.x + headerWidth_ + (currentFrame_ - scrollOffset_) * frameWidth;
    draw_list->AddLine(
        ImVec2(currentFrameX, canvas_pos.y),
        ImVec2(currentFrameX, canvas_pos.y + canvas_size.y),
        IM_COL32(255, 100, 100, 255), 2.0f);

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

        // キーフレーム描画
        for (uint32_t j = 0; j < track.keyframes.size(); j++) {
            TimeLineKeyFrame& kf = track.keyframes[j];

            if (kf.frame >= visibleFrameStart && kf.frame <= visibleFrameEnd) {
                float kfX = canvas_pos.x + headerWidth_ + (kf.frame - scrollOffset_) * frameWidth;
                float kfY = trackY + trackHeight_ / 2;

                // 持続時間の表示（横バー）
                if (kf.duration > 1.0f) {
                    float durationWidth = kf.duration * frameWidth;
                    draw_list->AddRectFilled(
                        ImVec2(kfX, kfY - 3),
                        ImVec2(kfX + durationWidth, kfY + 3),
                        IM_COL32(100, 150, 200, 128));
                }

                // キーフレームマーカー（ダイヤモンド）
                ImVec2 diamond[4] = {
                    ImVec2(kfX, kfY - 6),
                    ImVec2(kfX + 6, kfY),
                    ImVec2(kfX, kfY + 6),
                    ImVec2(kfX - 6, kfY)};

                ImU32 color = kf.isSelected ? IM_COL32(255, 255, 100, 255) : IM_COL32(255, 200, 50, 255);
                draw_list->AddConvexPolyFilled(diamond, 4, color);
                draw_list->AddPolyline(diamond, 4, IM_COL32(255, 255, 100, 255), ImDrawFlags_Closed, 1.5f);

                // ドラッグ&ドロップ処理
                HandleKeyFrameDragDrop(i, j, Vector2(kfX, kfY));
            }
        }

        trackY += trackHeight_;
    }

    // ドラッグ中の処理
    if (draggingTrackIndex_ >= 0 && draggingKeyIndex_ >= 0 && ImGui::IsMouseDragging(0)) {
        ImVec2 mouse_pos = ImGui::GetMousePos();
        float deltaX     = mouse_pos.x - (canvas_pos.x + headerWidth_ + (dragStartFrame_ - scrollOffset_) * frameWidth);
        int deltaFrame   = static_cast<int>(deltaX / frameWidth);
        int newFrame     = dragStartFrame_ + deltaFrame;

        if (newFrame >= 0 && newFrame <= endFrame_) {
            tracks_[draggingTrackIndex_].keyframes[draggingKeyIndex_].frame = newFrame;
        }
    }

    // ドラッグ終了
    if (ImGui::IsMouseReleased(0)) {
        if (draggingTrackIndex_ >= 0 && draggingKeyIndex_ >= 0) {
            // キーフレームを再ソート
            std::sort(tracks_[draggingTrackIndex_].keyframes.begin(),
                tracks_[draggingTrackIndex_].keyframes.end(),
                [](const TimeLineKeyFrame& a, const TimeLineKeyFrame& b) { return a.frame < b.frame; });

            tracks_[draggingTrackIndex_].keyframes[draggingKeyIndex_].isSelected = false;
        }
        draggingTrackIndex_ = -1;
        draggingKeyIndex_   = -1;
    }

    // インタラクション
    ImGui::SetCursorScreenPos(canvas_pos);
    ImGui::InvisibleButton("timeline_canvas", canvas_size);

    if (ImGui::IsItemClicked()) {
        ImVec2 mouse_pos = ImGui::GetMousePos();
        if (mouse_pos.x > canvas_pos.x + headerWidth_) {
            int clickedFrame = scrollOffset_ + (int)((mouse_pos.x - canvas_pos.x - headerWidth_) / frameWidth);
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