#pragma once
#include "TimelineDrawParameter.h"
#include "TimelineInteractionHandler.h"
#include "TimelineRenderer.h"
#include "Vector2.h"
#include <cstdint>
#include <functional>
#include <imgui.h>
#include <string>
#include <vector>

namespace KetaEngine {

// キーフレーム構造体
struct TimeLineKeyFrame {
    int32_t frame;
    float value;
    float duration;
    bool isSelected;
    bool isPositionLocked = false; // trueの場合、ドラッグによる位置変更を無効化
    std::string label;
};

// タイムライントラック
struct TimeLineTrack {
    std::string name;
    std::vector<TimeLineKeyFrame> keyframes;
    bool isExpanded = true;
    std::function<void(float)> onValueChanged;

    // キーフレーム右クリックコールバック
    std::function<void(int32_t trackIndex, int32_t keyIndex)> onKeyFrameRightClick;

    // トラック固有ID
    uint32_t id;
};

class TimelineDrawer {
public:
    TimelineDrawer()  = default;
    ~TimelineDrawer() = default;

    //*----------------------------- public Methods -----------------------------*//

    void Init(const std::string& name);
    void Draw(const std::string& name);

    /// <summary>
    /// 描画パラメータエディタウィンドウを描画
    /// </summary>
    void DrawParamEditor();

    /// <summary>
    /// トラック追加
    /// </summary>
    uint32_t AddTrack(const std::string& trackName,
        std::function<void(float)> callback = nullptr);

    /// <summary>
    /// トラック挿入
    /// </summary>
    uint32_t InsertTrack(uint32_t position, const std::string& trackName,
        std::function<void(float)> callback = nullptr);

    /// <summary>
    /// トラック削除
    /// </summary>
    bool RemoveTrack(uint32_t trackIndex);

    /// <summary>
    /// キーフレーム追加
    /// </summary>
    void AddKeyFrame(uint32_t trackIndex, int32_t frame, float value,
        float duration = 1.0f, const std::string& label = "");

    /// <summary>
    /// キーフレーム削除
    /// </summary>
    bool RemoveKeyFrame(uint32_t trackIndex, uint32_t keyIndex);

    /// <summary>
    /// キーフレームのラベルを設定
    /// </summary>
    bool SetKeyFrameLabel(uint32_t trackIndex, uint32_t keyIndex, const std::string& label);

    /// <summary>
    /// 指定フレームの値を取得
    /// </summary>
    float GetValueAtFrame(uint32_t trackIndex, int32_t frame) const;

    /// <summary>
    /// 指定トラックの最初のキーフレームのフレーム番号を取得
    /// </summary>
    int32_t GetFirstKeyFrameFrame(uint32_t trackIndex) const;

    /// <summary>
    /// 現在のフレームの値を全トラックに適用
    /// </summary>
    void ApplyCurrentFrame();

    /// <summary>
    /// キーフレームの右クリックコールバックを設定
    /// </summary>
    void SetKeyFrameRightClickCallback(uint32_t trackIndex,
        std::function<void(int32_t, int32_t)> callback);

    /// <summary>
    /// トラック数を取得
    /// </summary>
    size_t GetTrackCount() const { return tracks_.size(); }

private:
    //*---------------------------- private Methods ----------------------------*//

    // キーフレーム間の補間計算
    float InterpolateValue(const TimeLineKeyFrame& key1,
        const TimeLineKeyFrame& key2, int32_t frame) const;

    // ツールバー描画
    void DrawToolbar();

    // キーフレームのコンテキストメニュー処理（右クリックメニュー開閉）
    void DrawKeyFrameContextMenu(uint32_t trackIndex, uint32_t keyIndex,
        float kfX, float kfY);

private:
    //*---------------------------- private Variant ----------------------------*//

    // 描画パラメータ
    TimelineDrawParameter drawParam_;

    // トラック
    std::vector<TimeLineTrack> tracks_;

    std::function<void()> originalItemDrawCallBack_ = nullptr;

    int32_t currentFrame_ = 0;
    int32_t startFrame_   = 0;

    float zoom_       = 1.0f;
    int scrollOffset_ = 0;

    bool isPlaying_ = false;

    // 右クリックされたトラック
    int rightClickedTrackIndex_ = -1;

    // トラックID生成用
    uint32_t nextTrackId_ = 0;

    // 描画処理担当
    TimelineRenderer renderer_;
    // インタラクション処理担当
    TimelineInteractionHandler interactionHandler_;

public:
    //*----------------------------- getter Methods -----------------------------*//
    int32_t GetCurrentFrame() const { return currentFrame_; }
    int32_t GetEndFrame() const { return drawParam_.GetEndFrame(); }
    bool IsPlaying() const { return isPlaying_; }
    float GetZoom() const { return zoom_; }
    const std::vector<TimeLineTrack>& GetTracks() const { return tracks_; }
    std::vector<TimeLineTrack>& GetTracks() { return tracks_; }

    //*----------------------------- setter Methods -----------------------------*//
    void SetCurrentFrame(int frame) { currentFrame_ = frame; }
    void SetEndFrame(int frame) { drawParam_.SetEndFrame(frame); }
    void SetPlaying(bool playing) { isPlaying_ = playing; }
    void SetZoom(float zoom) { zoom_ = zoom; }
    void SetOriginalItemDrawCallBack(std::function<void()> callback) { originalItemDrawCallBack_ = callback; }
};

}; // KetaEngine
