#pragma once
#include "Easing.h"
#include <functional>
#include <memory>
#include <string>
#include <type_traits>

/// <summary>
/// イージングステップのインターフェース
/// </summary>
namespace KetaEngine {

class IEasingStep {
public:
    virtual ~IEasingStep() = default;

    /// <summary>
    /// JSONファイルから設定を適用
    /// </summary>
    /// <param name="fileName">JSONファイル名</param>
    virtual void ApplyFromJson(const std::string& fileName) = 0;

    /// <summary>
    /// ステップの更新
    /// </summary>
    /// <param name="deltaTime">デルタタイム</param>
    virtual void Update(float deltaTime) = 0;

    virtual void Reset()            = 0; //< ステップをリセット
    virtual bool IsFinished() const = 0; //< ステップが終了したか
    virtual bool IsPlaying() const  = 0; //< ステップが再生中か
};

/// <summary>
/// 型付きイージングステップクラス
/// </summary>
/// <typeparam name="T">イージングの値の型<float,Vector2,Vector3></typeparam>
template <typename T>
class EasingStep : public IEasingStep {
public:
    EasingStep();
    ~EasingStep() override = default;

    /// <summary>
    /// イージングオブジェクトを指定して構築
    /// </summary>
    /// <param name="easing">イージングオブジェクト</param>
    explicit EasingStep(std::unique_ptr<Easing<T>> easing);

    /// <summary>
    /// JSONファイルから設定を適用
    /// </summary>
    /// <param name="fileName">JSONファイル名</param>
    void ApplyFromJson(const std::string& fileName) override;

    /// <summary>
    /// ステップの更新
    /// </summary>
    /// <param name="deltaTime">デルタタイム</param>
    void Update(float deltaTime) override;

    void Reset() override; //< ステップをリセット
    bool IsFinished() const override; //< ステップが終了したか
    bool IsPlaying() const override; //< ステップが再生中か

private:
    std::unique_ptr<Easing<T>> easing_;

public:
    void SetAdaptValue(T* value);
    void SetBaseValue(const T& value);
    void SetOnWaitEndCallback(const std::function<void()>& callback);
    const T& GetValue() const { return easing_->GetValue(); }
};

}; // KetaEngine
