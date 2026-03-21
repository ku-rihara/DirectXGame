#include "TimelineDrawer.h"
#include "../../math/MathFunction.h"

using namespace KetaEngine;
#include <algorithm>
#include <cstdio>

void TimelineDrawer::Init(const std::string& name) {
    drawParam_.Init(name);

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
    isDraggingPlayHead_         = false;
}

void TimelineDrawer::DrawParamEditor() {
    /*  drawParam_.DrawImGui();*/
}

uint32_t TimelineDrawer::AddTrack(const std::string& trackName, std::function<void(float)> callback) {
    TimeLineTrack newTrack;
    // トラック名とコールバックを設定
    newTrack.name           = trackName;
    newTrack.onValueChanged = callback;
    // トラック固有IDを割り当て
    newTrack.id = nextTrackId_++;

    // トラックを末尾に追加し、Indexを返す
    tracks_.push_back(newTrack);
    return static_cast<uint32_t>(tracks_.size() - 1);
}

uint32_t TimelineDrawer::InsertTrack(uint32_t position, const std::string& trackName, std::function<void(float)> callback) {
    TimeLineTrack newTrack;
    // トラック名とコールバックを設定
    newTrack.name           = trackName;
    newTrack.onValueChanged = callback;
    // トラック固有IDを割り当て
    newTrack.id = nextTrackId_++;

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

    // トラックを削除
    tracks_.erase(tracks_.begin() + trackIndex);

    // ドラッグ中のトラックや右クリックされたトラックのインデックスを調整
    if (draggingTrackIndex_ == static_cast<int>(trackIndex)) {
        draggingTrackIndex_ = -1;
        draggingKeyIndex_   = -1;
    } else if (draggingTrackIndex_ > static_cast<int>(trackIndex)) {
        // 削除されたトラック以降のインデックスを-1ずらす
        draggingTrackIndex_--;
    }

    // duration ドラッグ中のトラックも同様に調整
    if (draggingDurationTrackIndex_ == static_cast<int>(trackIndex)) {
        draggingDurationTrackIndex_ = -1;
        draggingDurationKeyIndex_   = -1;
    } else if (draggingDurationTrackIndex_ > static_cast<int>(trackIndex)) {
        draggingDurationTrackIndex_--;
    }

    // 右クリックされたトラックのインデックスも調整
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
    // キーフレームのデータセット
    newKeyFrame.frame      = frame;
    newKeyFrame.value      = value;
    newKeyFrame.duration   = duration;
    newKeyFrame.isSelected = false;
    newKeyFrame.label      = label;

    // キーフレームを追加
    tracks_[trackIndex].keyframes.push_back(newKeyFrame);
}

bool TimelineDrawer::RemoveKeyFrame(uint32_t trackIndex, uint32_t keyIndex) {
    // インデックスの範囲チェック
    if (trackIndex >= tracks_.size()) {
        return false;
    }
    // キーフレームの範囲チェック
    if (keyIndex >= tracks_[trackIndex].keyframes.size()) {
        return false;
    }
    // キーフレームを削除
    tracks_[trackIndex].keyframes.erase(tracks_[trackIndex].keyframes.begin() + keyIndex);
    return true;
}

bool TimelineDrawer::SetKeyFrameLabel(uint32_t trackIndex, uint32_t keyIndex, const std::string& label) {
    // インデックスの範囲チェック
    if (trackIndex >= tracks_.size()) {
        return false;
    }

    // キーフレームの範囲チェック
    if (keyIndex >= tracks_[trackIndex].keyframes.size()) {
        return false;
    }

    // キーフレームのラベルセット
    tracks_[trackIndex].keyframes[keyIndex].label = label;
    return true;
}

float TimelineDrawer::GetValueAtFrame(uint32_t trackIndex, int32_t frame) const {
    // インデックスの範囲チェック
    if (trackIndex >= tracks_.size()) {
        return 0.0f;
    }

    // TrackのKeyFrameを取得
    const auto& keyframes = tracks_[trackIndex].keyframes;
    if (keyframes.empty()) {
        return 0.0f;
    }

    // フレーム位置に基づいて値を取得
    if (frame <= keyframes.front().frame) {
        return keyframes.front().value;
    }

    // フレーム位置がキーフレームの範囲内にある場合は補間して値を返す
    for (size_t i = 0; i < keyframes.size() - 1; ++i) {
        if (frame >= keyframes[i].frame && frame <= keyframes[i + 1].frame) {
            return InterpolateValue(keyframes[i], keyframes[i + 1], frame);
        }
    }

    // フレーム位置が最後のキーフレームを超えている場合は最後の値を返す
    return keyframes.back().value;
}

int32_t TimelineDrawer::GetFirstKeyFrameFrame(uint32_t trackIndex) const {
    // インデックスの範囲チェック
    if (trackIndex >= tracks_.size()) {
        return 0;
    }

    const auto& keyframes = tracks_[trackIndex].keyframes;
    if (keyframes.empty()) {
        return 0;
    }

    // キーフレームのフレームを返す
    return keyframes.front().frame;
}

float TimelineDrawer::InterpolateValue(const TimeLineKeyFrame& key1, const TimeLineKeyFrame& key2, int32_t frame) const {
    // キーフレーム間の線形補間
    float t = static_cast<float>(frame - key1.frame) / static_cast<float>(key2.frame - key1.frame);
    t       = std::clamp(t, 0.0f, 1.0f);
    return Lerp(key1.value, key2.value, t);
}

void TimelineDrawer::ApplyCurrentFrame() {
    // 現在のフレーム値を全Trackに適応
    for (uint32_t i = 0; i < tracks_.size(); i++) {
        if (tracks_[i].onValueChanged) {
            // 値が変わったらアプリ側でセットしたコールバック処理を呼び出す
            float value = GetValueAtFrame(i, currentFrame_);
            tracks_[i].onValueChanged(value);
        }
    }
}

void TimelineDrawer::SetKeyFrameRightClickCallback(uint32_t trackIndex, std::function<void(int32_t, int32_t)> callback) {
    if (trackIndex >= tracks_.size()) {
        return;
    }
    // アプリ側でセットした右クリックを押した時のコールバック処理を呼び出す
    tracks_[trackIndex].onKeyFrameRightClick = callback;
}

void TimelineDrawer::HandleKeyFrameDragDrop(uint32_t trackIndex, uint32_t keyIndex, const Vector2& keyPos) {
    // 位置ロック中のキーフレームはドラッグ不可
    if (tracks_[trackIndex].keyframes[keyIndex].isPositionLocked) {
        return;
    }

    const float rectSize = 8.0f; // 矩形の半分のサイズ
    if (ImGui::IsMouseClicked(0) && ImGui::IsMouseHoveringRect(ImVec2(keyPos.x - rectSize, keyPos.y - rectSize), ImVec2(keyPos.x + rectSize, keyPos.y + rectSize))) {

        // キーフレームドラッグ開始処理
        draggingTrackIndex_                                = trackIndex;
        draggingKeyIndex_                                  = keyIndex;
        dragStartFrame_                                    = tracks_[trackIndex].keyframes[keyIndex].frame;
        tracks_[trackIndex].keyframes[keyIndex].isSelected = true;
    }
}

void TimelineDrawer::HandleDurationDrag(uint32_t trackIndex, uint32_t keyIndex, float durationBarX, float trackY) {
    // デュレーションバーのドラッグ処理
    const float barHeight     = drawParam_.GetTrackHeight();
    const float barTopY       = trackY + (drawParam_.GetTrackHeight() - barHeight) / 2.0f;
    const float dragZoneWidth = 8.0f;

    // 　ドラッグ判定をデュレーションバーの左右端に配置
    ImVec2 dragZoneMin = ImVec2(durationBarX - dragZoneWidth, barTopY);
    ImVec2 dragZoneMax = ImVec2(durationBarX + dragZoneWidth, barTopY + barHeight);

    // デュレーションバードラッグ開始処理
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

    // ツールバー描画
    DrawToolbar();

    // -- 描画領域取得 --
    ImVec2 canvasPosImGui  = ImGui::GetCursorScreenPos(); // 現在のImGuiカーソル位置（スクリーン座標）
    ImVec2 canvasSizeImGui = ImGui::GetContentRegionAvail(); // ImGuiウィンドウ内の利用可能な描画領域のサイズ
    Vector2 canvasPos      = Vector2(canvasPosImGui.x, canvasPosImGui.y);
    Vector2 canvasSize     = Vector2(canvasSizeImGui.x, canvasSizeImGui.y);

    // ImGui描画リストを取得
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    // フレーム単位の幅
    const float frameWidth = 10.0f * zoom_;
    // 表示されているフレーム範囲を計算
    int visibleFrameStart = scrollOffset_;
    int visibleFrameEnd   = scrollOffset_ + static_cast<int>((canvasSize.x - drawParam_.GetHeaderWidth()) / frameWidth) + 1;
    float trackAreaHeight = drawParam_.GetRulerHeight() + tracks_.size() * drawParam_.GetTrackHeight();

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
    float currentFrameX = canvasPos.x + drawParam_.GetHeaderWidth() + (currentFrame_ - scrollOffset_) * frameWidth;
    drawList->AddLine(
        ImVec2(currentFrameX, canvasPos.y),
        ImVec2(currentFrameX, canvasPos.y + trackAreaHeight),
        IM_COL32(255, 100, 100, 255), 2.0f);

    UpdatePlayback();

    ImGui::End();
}

void TimelineDrawer::DrawToolbar() {
    // フレーム表示
    ImGui::Text("Frame: %d / %d", currentFrame_, drawParam_.GetEndFrame());
    ImGui::SameLine();

    // 再生の制御
    if (ImGui::Button(isPlaying_ ? "Pause" : "Play")) {
        if (!isPlaying_) {
            currentFrame_ = 0; // 再生開始時にフレームを0にリセット
        }
        isPlaying_ = !isPlaying_;
    }

    ImGui::SameLine();

    // 停止ボタン
    if (ImGui::Button("Stop")) {
        currentFrame_ = 0;
        isPlaying_    = false;
    }
    ImGui::SameLine();

    // ズームスライダー
    ImGui::SetNextItemWidth(100);
    ImGui::SliderFloat("Zoom", &zoom_, 0.1f, 5.0f);

    ImGui::Separator();
}

void TimelineDrawer::DrawBackground(::ImDrawList* drawList, const Vector2& canvasPos, const Vector2& canvasSize) {
    // タイムラインの背景全体を塗りつぶす
    drawList->AddRectFilled(
        ImVec2(canvasPos.x, canvasPos.y),
        ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y),
        IM_COL32(30, 30, 30, 255));
}

void TimelineDrawer::DrawRulerAndGrid(::ImDrawList* drawList, const Vector2& canvasPos,
    const Vector2& canvasSize, float frameWidth, float trackAreaHeight,
    int visibleFrameStart, int visibleFrameEnd) {

    // ルーラー背景描画
    ImVec2 rulerStart = ImVec2(canvasPos.x, canvasPos.y);
    ImVec2 rulerEnd   = ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + drawParam_.GetRulerHeight());
    drawList->AddRectFilled(rulerStart, rulerEnd, IM_COL32(50, 50, 50, 255));

    // フレーム番号とグリッド
    for (int frame = visibleFrameStart; frame <= visibleFrameEnd && frame <= drawParam_.GetEndFrame(); frame++) {
        float x = canvasPos.x + drawParam_.GetHeaderWidth() + (frame - scrollOffset_) * frameWidth;

        // 5フレームごとに太い線を描画し、フレーム番号を表示
        if (frame % 5 == 0) {
            // グリッド線の描画
            drawList->AddLine(
                ImVec2(x, canvasPos.y + drawParam_.GetRulerHeight()),
                ImVec2(x, canvasPos.y + trackAreaHeight),
                IM_COL32(60, 60, 60, 255), 1.0f);

            // フレーム番号の描画
            char frameText[8];
            std::snprintf(frameText, sizeof(frameText), "%d", frame);
            drawList->AddText(ImVec2(x + 2, canvasPos.y + 5), IM_COL32(200, 200, 200, 255), frameText);
        } else {
            // 1フレームごとに細い線を描画
            drawList->AddLine(
                ImVec2(x, canvasPos.y + drawParam_.GetRulerHeight()),
                ImVec2(x, canvasPos.y + trackAreaHeight),
                IM_COL32(45, 45, 45, 255), 1.0f);
        }
    }
}

void TimelineDrawer::DrawTracks(::ImDrawList* drawList, const Vector2& canvasPos,
    const Vector2& canvasSize, float frameWidth,
    int visibleFrameStart, int visibleFrameEnd) {

    // トラックの描画開始Y位置(ルーラーから下)
    float trackY = canvasPos.y + drawParam_.GetRulerHeight();

    // 各トラックの描画
    for (uint32_t i = 0; i < tracks_.size(); i++) {
        TimeLineTrack& track = tracks_[i];

        // トラックヘッダーの始点終点
        ImVec2 headerStart = ImVec2(canvasPos.x, trackY);
        ImVec2 headerEnd   = ImVec2(canvasPos.x + drawParam_.GetHeaderWidth(), trackY + drawParam_.GetTrackHeight());

        // ヘッダー背景と枠線、テキスト描画
        drawList->AddRectFilled(headerStart, headerEnd, IM_COL32(40, 40, 40, 255)); // 背景
        drawList->AddRect(headerStart, headerEnd, IM_COL32(70, 70, 70, 255));       // 枠線
        drawList->AddText(ImVec2(canvasPos.x + 10, trackY + 8), IM_COL32(255, 255, 255, 255), track.name.c_str()); // 文字

        // トラックライン背景の始点終点
        ImVec2 lineStart = ImVec2(canvasPos.x + drawParam_.GetHeaderWidth(), trackY);
        ImVec2 lineEnd   = ImVec2(canvasPos.x + canvasSize.x, trackY + drawParam_.GetTrackHeight());

        // ヘッダー背景と枠線描画
        drawList->AddRectFilled(lineStart, lineEnd, (i % 2 == 0) ? IM_COL32(35, 35, 35, 255) : IM_COL32(30, 30, 30, 255));
        drawList->AddLine(ImVec2(canvasPos.x, trackY + drawParam_.GetTrackHeight()),
            ImVec2(canvasPos.x + canvasSize.x, trackY + drawParam_.GetTrackHeight()), IM_COL32(50, 50, 50, 255));

        // キーフレーム描画
        for (uint32_t j = 0; j < track.keyframes.size(); j++) {
            DrawKeyFrame(drawList, i, j, canvasPos, frameWidth, visibleFrameStart, visibleFrameEnd, trackY);
        }

        // 次のトラックのY位置へ加算
        trackY += drawParam_.GetTrackHeight();
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

    float kfX = canvasPos.x + drawParam_.GetHeaderWidth() + (kf.frame - scrollOffset_) * frameWidth;
    float kfY = trackY + drawParam_.GetTrackHeight() / 2;

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
        float barHeight     = drawParam_.GetTrackHeight() * 0.8f;
        float barTopY       = trackY + (drawParam_.GetTrackHeight() - barHeight) / 2.0f;

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
        // 開始位置からのフレームの変化量を計算
        float deltaX    = mousePos.x - (canvasPos.x + drawParam_.GetHeaderWidth() + (dragStartFrame_ - scrollOffset_) * frameWidth);
        int deltaFrame  = static_cast<int>(deltaX / frameWidth);
        int newFrame    = dragStartFrame_ + deltaFrame;

        // 動かしたフレームを適応
        if (newFrame >= 0 && newFrame <= drawParam_.GetEndFrame()) {
            tracks_[draggingTrackIndex_].keyframes[draggingKeyIndex_].frame = newFrame;
        }
    }

    // ドラッグ中の処理(duration)
    if (draggingDurationTrackIndex_ >= 0 && draggingDurationKeyIndex_ >= 0 && ImGui::IsMouseDragging(0)) {
        ImVec2 mousePos   = ImGui::GetMousePos();
        auto& kf          = tracks_[draggingDurationTrackIndex_].keyframes[draggingDurationKeyIndex_];
        float kfX         = canvasPos.x + drawParam_.GetHeaderWidth() + (kf.frame - scrollOffset_) * frameWidth;
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

    float endFrameX  = canvasPos.x + drawParam_.GetHeaderWidth() + (drawParam_.GetEndFrame() - scrollOffset_) * frameWidth;
    float canvasEndX = canvasPos.x + drawParam_.GetHeaderWidth();

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

    // 再生ヘッドの現在位置を計算
    float currentFrameX          = canvasPos.x + drawParam_.GetHeaderWidth() + (currentFrame_ - scrollOffset_) * frameWidth;
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
        if (mousePos.x > canvasPos.x + drawParam_.GetHeaderWidth()) {
            int clickedFrame = scrollOffset_ + static_cast<int>((mousePos.x - canvasPos.x - drawParam_.GetHeaderWidth()) / frameWidth);
            if (clickedFrame >= 0 && clickedFrame <= drawParam_.GetEndFrame()) {
                currentFrame_ = clickedFrame;
                ApplyCurrentFrame();
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
            if (scrollOffset_ > drawParam_.GetEndFrame() - 10) {
                scrollOffset_ = drawParam_.GetEndFrame() - 10;
            }
        }
    }
}

void TimelineDrawer::UpdatePlayback() {
    if (isPlaying_) {
        currentFrame_++;
        if (currentFrame_ > drawParam_.GetEndFrame()) {
            currentFrame_ = 0;
        }
        ApplyCurrentFrame();
    }
}