#pragma once
#include "Editor/EasingCreator/EasingParameterData.h"
#include "Vector2Proxy.h"
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

/// <summary>
/// イージングを扱うのテンプレートクラス
/// </summary>
/// <typeParam name="T">イージング対象の型(float, Vector2, Vector3)</typeParam>
namespace KetaEngine {
template <typename T>
class Easing {
public:
    Easing()  = default;
    ~Easing() = default;

    // 初期化(ファイルロード)、更新
    void Init(const std::string& adaptFile);
    void Update(float deltaTime);

    /// <summary>
    /// イージングパラメータ設定
    /// </summary>
    /// <param name="easingParam">イージングパラメータ</param>
    void SettingValue(const EasingParameter<T>& easingParam);

    /// <summary>
    /// JSONファイルから適用
    /// </summary>
    /// <param name="fileName">ファイル名</param>
    void ApplyFromJson(const std::string& fileName);

    /// <summary>
    /// 変数に適用
    /// </summary>
    /// <param name="value">適用先の値</param>
    void SetAdaptValue(T* value);

    /// <summary>
    /// Vector2の軸に適用(float用)
    /// </summary>
    /// <param name="value">適用先のVector2</param>
    template <typename U = T>
    typename std::enable_if_t<std::is_same_v<U, float>, void>
    SetAdaptValue(Vector2* value);

    /// <summary>
    /// Vector3の軸に適用(float用)
    /// </summary>
    /// <param name="value">適用先のVector3</param>
    template <typename U = T>
    typename std::enable_if_t<std::is_same_v<U, float>, void>
    SetAdaptValue(Vector3* value);

    /// <summary>
    /// Vector3の軸に適用(Vector2用)
    /// </summary>
    /// <param name="value">適用先のVector3</param>
    template <typename U = T>
    typename std::enable_if_t<std::is_same_v<U, Vector2>, void>
    SetAdaptValue(Vector3* value);

    // 　イージング終了コールバック、いーじんぐ待機終了コールバック
    void SetOnFinishCallback(const std::function<void()>& callback) { onFinishCallback_ = callback; }
    void SetOnWaitEndCallback(const std::function<void()>& callback) { onWaitEndCallback_ = callback; }

    void Reset();           //< リセット
    void ResetStartValue(); //< 開始値にリセット
    void ApplyForImGui();   //< ImGuiで適用

private:
    void CalculateValue(); //< イージング値計算
    void FinishBehavior(); //< 終了時の動作
    void ChangeAdaptAxis(); //< 適用軸変更
    void FilePathChangeForType(); //< ファイルパスを型に応じて変更
    bool IsEasingStarted() const; //< イージング開始判定

private:
    EasingType type_                       = EasingType::InSine;
    EasingFinishValueType finishValueType_ = EasingFinishValueType::End;

    AdaptFloatAxisType adaptFloatAxisType_     = AdaptFloatAxisType::X;
    AdaptVector2AxisType adaptVector2AxisType_ = AdaptVector2AxisType::XY;

private:
    T startValue_;
    T endValue_;
    T baseValue_;
    T* currentOffset_;

    float maxTime_     = 0.0f;
    float currentTime_ = 0.0f;
    float waitTimeMax_ = 0.0f;
    float waitTime_    = 0.0f;

    float startTimeOffset_        = 0.0f;
    float finishTimeOffset_       = 0.0f;
    float currentStartTimeOffset_ = 0.0f;

    float amplitude_ = 0.0f;
    float period_    = 0.0f;
    float backRatio_ = 0.0f;

    bool isFinished_ = false;
    bool isPlaying_  = false;

private:
    const std::string FilePath_ = "Resources/EasingParameter/";

    int32_t selectedFileIndex_;
    std::vector<std::string> easingFiles_;
    std::string currentAppliedFileName_;
    std::string filePathForType_;
    std::string currentSelectedFileName_;
    std::string easingName_;

    AdaptVector2AxisType oldTypeVector2_;
    AdaptFloatAxisType oldTypeFloat_;
    Vector2* adaptTargetVec2_ = nullptr;
    Vector3* adaptTargetVec3_ = nullptr;
    std::unique_ptr<IVector2Proxy> vector2Proxy_;

    std::function<void()> onFinishCallback_;
    std::function<void()> onWaitEndCallback_;

    bool isStartEndReverse_ = false;

public:
    /// -------------------------------------------------------------------------
    /// Getter methods
    /// -------------------------------------------------------------------------
    const T& GetValue() const { return *currentOffset_; }
    const T& GetEndValue() const { return endValue_; }
    const T& GetStartValue() const { return startValue_; }
    bool IsFinished() const { return isFinished_; }
    bool IsPlaying() const { return isPlaying_; }
    const std::string& GetCurrentAppliedFileName() const { return currentAppliedFileName_; }
    float GetCurrentEaseTime() const { return currentTime_; }
    /// -------------------------------------------------------------------------
    /// Setter methods
    /// -------------------------------------------------------------------------
    void SetStartValue(const T& value) { startValue_ = value; }
    void SetEndValue(const T& value) { endValue_ = value; }
    void SetCurrentOffset(const T& value);
    void SetBaseValue(const T& value) { baseValue_ = value; }
    void SetType(const EasingType& type) { type_ = type; }
    void SetMaxTime(float time) { maxTime_ = time; }
    void SetIsStartEndReverse(bool isReverse) { isStartEndReverse_ = isReverse; }
    void SetFinishTimeOffset(float time) { finishTimeOffset_ = time; }
};

}; // KetaEngine
