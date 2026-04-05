#include "TimelineDrawer.h"
#include "../../math/MathFunction.h"

using namespace KetaEngine;
#include <algorithm>
#include <cstdio>

void TimelineDrawer::Init(const std::string& name) {
    drawParam_.Init(name);

    tracks_.clear();
    currentFrame_           = 0;
    scrollOffset_           = 0;
    isPlaying_              = false;
    rightClickedTrackIndex_ = -1;
    nextTrackId_            = 0;

    // 描画クラス初期化
    renderer_.Init(&drawParam_, &tracks_, &currentFrame_, &scrollOffset_);
    // インタラクションクラス初期化
    interactionHandler_.Init(&drawParam_, &tracks_);
    interactionHandler_.Reset();
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

    // 右クリックされたトラックのインデックスも調整
    if (rightClickedTrackIndex_ == static_cast<int>(trackIndex)) {
        rightClickedTrackIndex_ = -1;
    } else if (rightClickedTrackIndex_ > static_cast<int>(trackIndex)) {
        rightClickedTrackIndex_--;
    }

    // InteractionHandler 側のドラッグインデックスも調整
    interactionHandler_.AdjustIndicesAfterTrackRemoval(static_cast<int32_t>(trackIndex));

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
    renderer_.DrawBackground(drawList, canvasPos, canvasSize);
    // ルーラーとグリッドの描画
    renderer_.DrawRulerAndGrid(drawList, canvasPos, canvasSize, frameWidth, trackAreaHeight, visibleFrameStart, visibleFrameEnd);
    // トラックの描画（視覚部分のみ）
    renderer_.DrawTracks(drawList, canvasPos, canvasSize, frameWidth, visibleFrameStart, visibleFrameEnd);

    // インタラクションハンドラーに現在状態をセット
    interactionHandler_.SetCurrentFrame(currentFrame_);
    interactionHandler_.SetScrollOffset(scrollOffset_);
    interactionHandler_.SetIsPlaying(isPlaying_);

    // キーフレームごとのインタラクション・コンテキストメニュー処理
    float trackY = canvasPos.y + drawParam_.GetRulerHeight();
    for (uint32_t i = 0; i < tracks_.size(); i++) {
        for (uint32_t j = 0; j < tracks_[i].keyframes.size(); j++) {
            const auto& kf = tracks_[i].keyframes[j];

            if (kf.frame < visibleFrameStart || kf.frame > visibleFrameEnd) {
                continue;
            }

            float kfX = canvasPos.x + drawParam_.GetHeaderWidth() + (kf.frame - scrollOffset_) * frameWidth;
            float kfY = trackY + drawParam_.GetTrackHeight() / 2;

            // キーフレームドラッグ処理
            interactionHandler_.HandleKeyFrameDragDrop(i, j, Vector2(kfX, kfY));

            // デュレーションバードラッグ処理
            if (kf.duration > 1.0f) {
                float durationEndX = kfX + kf.duration * frameWidth;
                interactionHandler_.HandleDurationDrag(i, j, durationEndX, trackY);
            }

            // キーフレームコンテキストメニュー
            DrawKeyFrameContextMenu(i, j, kfX, kfY);
        }
        trackY += drawParam_.GetTrackHeight();
    }

    // ドラッグ処理の更新
    interactionHandler_.HandleDragUpdates(canvasPos, frameWidth);
    // フレームの終点の点線描画
    renderer_.DrawEndFrameLine(drawList, canvasPos, frameWidth, trackAreaHeight);
    // キャンバスのインタラクション
    interactionHandler_.HandleCanvasInteraction(canvasPos, canvasSize, frameWidth, trackAreaHeight,
        [this]() { ApplyCurrentFrame(); });

    // 現在フレームインジケーター（再生ヘッド）描画
    renderer_.DrawPlayHead(drawList, canvasPos, frameWidth, trackAreaHeight);

    // 再生状態の更新
    interactionHandler_.UpdatePlayback([this]() { ApplyCurrentFrame(); });

    // インタラクションハンドラーから変更後の状態を取得
    currentFrame_ = interactionHandler_.GetCurrentFrame();
    scrollOffset_ = interactionHandler_.GetScrollOffset();
    isPlaying_    = interactionHandler_.GetIsPlaying();

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

void TimelineDrawer::DrawKeyFrameContextMenu(uint32_t trackIndex, uint32_t keyIndex, float kfX, float kfY) {
    std::string popupId = "KeyFrameContextMenu_" + std::to_string(trackIndex) + "_" + std::to_string(keyIndex);

    // キーフレーム右クリック
    if (ImGui::IsMouseClicked(1) && ImGui::IsMouseHoveringRect(ImVec2(kfX - 8, kfY - 8), ImVec2(kfX + 8, kfY + 8))) {
        ImGui::OpenPopup(popupId.c_str());
    }

    // キーフレームコンテキストメニュー
    if (ImGui::BeginPopup(popupId.c_str())) {
        if (ImGui::MenuItem("横棒を1フレーム伸ばす")) {
            tracks_[trackIndex].keyframes[keyIndex].duration += 1.0f;
        }

        ImGui::Separator();

        if (tracks_[trackIndex].onKeyFrameRightClick) {
            tracks_[trackIndex].onKeyFrameRightClick(trackIndex, keyIndex);
        }

        if (ImGui::MenuItem("Delete KeyFrame")) {
            RemoveKeyFrame(trackIndex, keyIndex);
        }
        ImGui::EndPopup();
    }
}
