#pragma once
#include "RailData.h"
#include "Vector3.h"
#include <memory>
#include <string>

/// <summary>
/// レールプレイヤー
/// </summary>
class RailPlayer {
public:
    RailPlayer()  = default;
    ~RailPlayer() = default;

    void Init(); //< 初期化
    void Stop(); //< レール停止

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="speed">移動速度</param>
    /// <param name="mode">座標モード</param>
    /// <param name="direction">方向</param>
    void Update(const float& speed, const RailData::PositionMode& mode = RailData::PositionMode::WORLD, const Vector3& direction = {1.0f, 1.0f, 1.0f});

    /// <summary>
    /// レールの再生
    /// </summary>
    /// <param name="railName">レール名</param>
    void Play(const std::string& railName);

    /// <summary>
    /// 親トランスフォームの設定
    /// </summary>
    /// <param name="parent">親トランスフォーム</param>
    void SetParent(WorldTransform* parent);

private:
    std::unique_ptr<RailData> railData_;
    Vector3 currentPosition_ = {0.0f, 0.0f, 0.0f};

public:
    const Vector3& GetCurrentPosition() const { return currentPosition_; }
    RailData* GetRailData() { return railData_.get(); }
    bool IsPlaying() const { return railData_ ? railData_->IsPlaying() : false; }
};