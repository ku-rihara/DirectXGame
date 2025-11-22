#pragma once
#include "Easing/Easing.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "Editor/RailEditor/RailPlayer.h"
#include "utility/FileSelector/FileSelector.h"
#include "Vector3.h"
#include <array>
#include <cstdint>
#include <memory>
#include <string>

/// <summary>
/// オブジェクトイージングアニメーションキーフレーム
/// </summary>
class ObjEaseAnimationKeyFrame {
public:
    enum class TransformType {
        Scale,
        Rotation,
        Translation,
        Count
    };

     enum class PlayState {
        STOPPED,
        PLAYING,
        RETURNING
    };

    enum class TimeMode {
        DELTA_TIME      = 0,
        DELTA_TIME_RATE = 1
    };

    struct TransformParam {
        bool isActive         = false;
        bool isReturnToOrigin = false;
        bool useRail          = false;

        Vector3 endValue = Vector3::ZeroVector();

        // 通常再生パラメータ
        float maxTime    = 1.0f;
        int32_t easeType = 0;

        // 戻り動作パラメータ
        float returnMaxTime = 1.0f;
        int32_t returnEaseType = 0;

        std::string railFileName;

        // 現在のオフセット値
        Vector3 currentOffset = Vector3::ZeroVector();
        Vector3 startValue    = Vector3::ZeroVector();
        // イージング
        Easing<Vector3> ease;
        Easing<Vector3> returnEase;
    };

    struct RailFileSelector {
        FileSelector selector;
    };

public:
    ObjEaseAnimationKeyFrame()  = default;
    ~ObjEaseAnimationKeyFrame() = default;

    // 初期化、更新
    void Init(const std::string& animationName,const std::string&categoryName, const int32_t& keyNumber);
    void Update(const float& speedRate);

    // リセット、終了判定
    void Reset();
    bool IsFinished() const;

    // データ読み込み、保存、編集
    void LoadData();
    void SaveData();
    void AdjustParam();

    /// <summary>
    /// 開始値を設定
    /// </summary>
    void SetStartValues(const Vector3& scale, const Vector3& rotation, const Vector3& translation);

private:
    const char* GetSRTName(const TransformType& type) const;

    void RegisterParams();
    void LoadParams();
    void AdaptEaseParam();
    void AdaptValueSetting();

    // 変換更新、戻り更新
    void UpdatePlay(const float& deltaTime);
    void UpdateReturn(const float& deltaTime);
    void CheckFinish();

    // 戻り動作開始
    void StartReturn();

    void ImGuiTransformParam(const char* label, TransformParam& param, const TransformType& type);
    void TimeModeSelector(const char* label, int32_t& target);

    void CheckPlayFinishAndStartReturn();

private:
    GlobalParameter* globalParameter_;
    std::string groupName_;
    std::string folderPath_;
    int32_t currentKeyFrameIndex_ = -1;

    // transformParam
    std::array<TransformParam, static_cast<size_t>(TransformType::Count)> transformParams_;
     
    // time
    float timePoint_  = 1.0f;
    int32_t timeMode_ = static_cast<int32_t>(TimeMode::DELTA_TIME_RATE);

    // rail
    std::unique_ptr<RailPlayer> railPlayer_;
    RailFileSelector railFileSelector_;

    // 再生状態
    PlayState playState_ = PlayState::STOPPED;

    std::vector<const char*> TimeModeLabels = {
        "DeltaTime (No TimeScale)",
        "DeltaTimeRate (With TimeScale)"};

public:
    const float& GetTimePoint() const { return timePoint_; }
    const Vector3& GetCurrentScale() const { return transformParams_[static_cast<size_t>(TransformType::Scale)].currentOffset; }
    const Vector3& GetCurrentRotation() const { return transformParams_[static_cast<size_t>(TransformType::Rotation)].currentOffset; }
    const Vector3& GetCurrentTranslation() const { return transformParams_[static_cast<size_t>(TransformType::Translation)].currentOffset; }

    const Vector3& GetEditScale() const { return transformParams_[static_cast<size_t>(TransformType::Scale)].endValue; }
    const Vector3& GetEditRotation() const { return transformParams_[static_cast<size_t>(TransformType::Rotation)].endValue; }
    const Vector3& GetEditTranslation() const { return transformParams_[static_cast<size_t>(TransformType::Translation)].endValue; }

    RailPlayer* GetRailPlayer() { return railPlayer_.get(); }
    bool IsUsingRail() const;

    void SetTimePoint(const float& timePoint) { timePoint_ = timePoint; }
};