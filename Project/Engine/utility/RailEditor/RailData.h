#pragma once
#include "Easing/Easing.h"
#include "utility/ParameterEditor/GlobalParameter.h"
#include "utility/RailEditor/Rail.h"
#include "Vector3.h"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

/// <summary>
/// レールデータ
/// </summary>
class RailData {
public:
    enum class PlayState {
        STOPPED,
        PLAYING,
        PAUSED
    };

    enum class PositionMode {
        WORLD,
        LOCAL,
    };

public:
    RailData()  = default;
    ~RailData() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="railName">レール名</param>
    void Init(const std::string& railName);

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="speed">移動速度</param>
    /// <param name="mode">座標モード</param>
    /// <param name="direction">方向</param>
    void Update(const float& speed, const PositionMode& mode = PositionMode::WORLD, const Vector3& direction = {1.0f, 1.0f, 1.0f});

    void AdjustParam(); //< パラメータ調整
    void Play(); //< 再生
    void Stop(); //< 停止
    void Reset(); //< リセット
    void LoadData(); //< データロード
    void SaveData(); //< データセーブ
    bool IsPlaying() const; //< 再生中か
    bool IsFinished() const; //< 終了したか

    /// <summary>
    /// 制御点の追加
    /// </summary>
    /// <param name="position">座標</param>
    void AddPoint(const Vector3& position);

    /// <summary>
    /// 制御点の削除
    /// </summary>
    /// <param name="index">インデックス</param>
    void RemovePoint(size_t index);

private:
    void RegisterParams(); //< パラメータのバインド
    void LoadParams(); //< パラメータ取得
    void ResetParams(); //< パラメータリセット
    void RoopOrStop(); //< ループまたは停止
    void ImGuiControlPoints(); //< 制御点のImGui編集

private:
    GlobalParameter* globalParameter_;
    std::string groupName_;
    std::string folderPath_ = "RailEditor";

    std::unique_ptr<Rail> rail_;

    // 制御点データ
    std::vector<Vector3> controlPoints_;
    Vector3 tempAddPosition_ = {0.0f, 0.0f, 0.0f};

    float maxTime_    = 1.0f; 
    float startTime_  = 0.0f; 
    int32_t easeType_ = 0;
    bool isLoop_      = true;

    float elapsedTime_ = 0.0f; 
    Easing<float> timeEase_;
    float easedTime_ = 0.0f;

    PlayState playState_ = PlayState::STOPPED;

    Vector3 currentPosition_ = Vector3::ZeroVector();
    Vector3 scale_           = {1.0f, 1.0f, 1.0f};
    Vector3 direction_       = {1.0f, 1.0f, 1.0f};

    WorldTransform* parentTransform_ = nullptr;

    bool showControls_ = true;

    const std::string dyrectrypath_ = "Resources/ControlPoint/";

public:
    std::string GetGroupName() const { return groupName_; }
    Vector3 GetCurrentPosition() const { return currentPosition_; }
    bool GetIsLoop() const { return isLoop_; }
    Rail* GetRail() { return rail_.get(); }
    const std::vector<Vector3>& GetControlPoints() const { return controlPoints_; }
    WorldTransform* GetParentTransform() const { return parentTransform_; }

    void SetIsLoop(bool loop) { isLoop_ = loop; }
    void SetScale(const Vector3& scale) { scale_ = scale; }
    void SetParent(WorldTransform* parent) { parentTransform_ = parent; }
    void SetDirection(const Vector3& direction) { direction_ = direction; }
};