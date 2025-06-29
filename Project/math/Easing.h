#pragma once
#include "EasingFunction.h"
#include "Vector2Proxy.h"
#include "utility/EasingCreator/EasingParameterData.h"
// std
#include <cstdint>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>
#include <functional>

//template <typename T>
//class EasingCreator;
//class EasingEditor;
template <typename T>
class Easing {
public:
    Easing() = default;

    void Init(const std::string& name);

    void Reset();

    //  Easing setting
    void SettingValue(const EasingParameter<T>& easingParam);

    // 適応
    void ApplyFromJson(const std::string& fileName);
    void ApplyForImGui();
    void FilePathChangeForType();

    void SaveAppliedJsonFileName();
    void LoadAndApplyFromSavedJson();
    void ChangeAdaptAxis();

    // イージング更新
    void Update(float deltaTime);

    // 変数に適応
    void SetAdaptValue(T* value);

    template <typename U = T>
    typename std::enable_if_t<std::is_same_v<U, float>, void>
    SetAdaptValue(Vector2* value);

    template <typename U = T>
    typename std::enable_if_t<std::is_same_v<U, float>, void>
    SetAdaptValue(Vector3* value);

    template <typename U = T>
    typename std::enable_if_t<std::is_same_v<U, Vector2>, void>
    SetAdaptValue(Vector3* value);

    void SetValue(const T& value);

private:
    // イージング計算
    void CalculateValue();
    // 終了処理
    void FinishBehavior();

public:
    // メンバ変数
    EasingType type_                       = EasingType::InSine;
    EasingFinishValueType finishValueType_ = EasingFinishValueType::End;

    AdaptFloatAxisType adaptFloatAxisType_     = AdaptFloatAxisType::X;
    AdaptVector2AxisType adaptVector2AxisType_ = AdaptVector2AxisType::XY;

    T startValue_;
    T endValue_;
    T* currentValue_;

    float maxTime_     = 0.0f;
    float currentTime_ = 0.0f;

    bool isRunning_  = false;
    bool isFinished_ = false;

    // amplitude用Parameter
    float amplitude_ = 0.0f;
    float period_    = 0.0f;
    float backRatio_ = 0.0f;

private:
    int32_t selectedFileIndex_;
    AdaptVector2AxisType oldTypeVector2_;
    AdaptFloatAxisType oldTypeFloat_;

    std::vector<std::string> easingFiles_;

    const std::string FilePath_ = "Resources/EasingParameter/";
    std::string currentAppliedFileName_;
    std::string filePathForType_;
    std::string currentSelectedFileName_;
    std::string easingName_;

    Vector2* adaptTargetVec2_ = nullptr;
    Vector3* adaptTargetVec3_ = nullptr;
    std::unique_ptr<IVector2Proxy> vector2Proxy_;

  std::function<void()> onFinishCallback_;

public:
    /// -------------------------------------------------------------------------
    /// Getter methods
    /// -------------------------------------------------------------------------
    const T& GetValue() const { return *currentValue_; }
    bool IsFinished() const { return isFinished_; }
    bool IsRunning() const { return isRunning_; }
    std::string GetCurrentAppliedFileName() const { return currentAppliedFileName_; }

    /// -------------------------------------------------------------------------
    /// Setter methods
    /// -------------------------------------------------------------------------
  /*  void SetEditor(EasingEditor* editor);*/
    void SetFinishValueType(const EasingFinishValueType& type) { finishValueType_ = type; }
    void SetOnFinishCallback(const std::function<void()>& callback) { onFinishCallback_ = callback; }
};