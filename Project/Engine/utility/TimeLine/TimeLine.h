#pragma once
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace KetaEngine {

// キーフレーム構造体
struct TimeLineKeyFrame {
    int32_t frame;
    float value;
    float duration;
    bool isSelected;
};

// タイムライントラック
struct TimeLineTrack {
    std::string name;
    std::vector<TimeLineKeyFrame> keyframes;
    bool isExpanded = true;
    std::function<void(float)> onValueChanged;
};

struct ImVec2;

class TimeLine {
public:
    TimeLine()  = default;
    ~TimeLine() = default;

    //*----------------------------- public Methods -----------------------------*//

    // 初期化、描画
    void Init();
    void Draw();

    /// <summary>
    /// トラック追加
    /// </summary>
    /// <param name="trackName">トラック名</param>
    /// <param name="callback">値変更時のコールバック</param>
    /// <returns>追加されたTrackのIndex</returns>
    uint32_t AddTrack(const std::string& trackName, std::function<void(float)> callback = nullptr);

    /// <summary>
    /// キーフレーム追加
    /// </summary>
    /// <param name="trackIndex">トラックのインデックス</param>
    /// <param name="frame">フレーム位置</param>
    /// <param name="value">値</param>
    /// <param name="duration">持続時間（フレーム数）</param>
    void AddKeyFrame(uint32_t trackIndex, int32_t frame, float value, float duration = 1.0f);

    /// <summary>
    /// 指定フレームの値を取得
    /// </summary>
    /// <param name="trackIndex">トラックのインデックス</param>
    /// <param name="frame">フレーム位置</param>
    /// <returns>補間された値</returns>
    float GetValueAtFrame(uint32_t trackIndex, int32_t frame) const;

    /// <summary>
    /// 現在のフレームの値を全トラックに適用
    /// </summary>
    void ApplyCurrentFrame();

private:
    //*---------------------------- private Methods ----------------------------*//

    /// <summary>
    /// キーフレームのドラッグ&ドロップ処理
    /// </summary>
    void HandleKeyFrameDragDrop(uint32_t trackIndex, uint32_t keyIndex, const ImVec2& keyPos);

    /// <summary>
    /// 2つのキーフレーム間で補間
    /// </summary>
    float InterpolateValue(const TimeLineKeyFrame& key1, const TimeLineKeyFrame& key2, int32_t frame) const;

private:
    //*---------------------------- private Variant ----------------------------*//

    std::vector<TimeLineTrack> tracks_; // トラックリスト

    int32_t currentFrame_ = 0;   // 現在のフレーム
    int32_t startFrame_   = 0;   // 開始フレーム
    int32_t endFrame_     = 300; // 終了フレーム

    float zoom_ = 1.0f;    // ズームレベル
    int scrollOffset_ = 0; // スクロールオフセット

    bool isPlaying_   = false; // 再生中フラグ

     // UI設定
    const float headerWidth_ = 150.0f; // ヘッダー幅
    const float trackHeight_ = 30.0f;  // トラック高さ
    const float rulerHeight_ = 25.0f;  // ルーラー高さ

    // ドラッグ&ドロップ用
    int draggingTrackIndex_ = -1; // ドラッグ中のトラックインデックス
    int draggingKeyIndex_   = -1; // ドラッグ中のキーインデックス
    int dragStartFrame_     = 0;  // ドラッグ開始フレーム

public:
    //*----------------------------- getter Methods -----------------------------*//
    int32_t GetCurrentFrame() const { return currentFrame_; }
    int32_t GetEndFrame() const { return endFrame_; }
    bool IsPlaying() const { return isPlaying_; }
    float GetZoom() const { return zoom_; }

    //*----------------------------- setter Methods -----------------------------*//
    void SetCurrentFrame(int frame) { currentFrame_ = frame; }
    void SetEndFrame(int frame) { endFrame_ = frame; }
    void SetPlaying(bool playing) { isPlaying_ = playing; }
    void SetZoom(float zoom) { zoom_ = zoom; }
};

}; // KetaEngine
