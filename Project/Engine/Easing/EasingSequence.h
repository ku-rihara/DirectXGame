#pragma once
#include "EasingStep.h"
#include <cassert>
#include <functional>
#include <memory>
#include <vector>

/// <summary>
/// イージングステップのシーケンス管理クラス
/// </summary>
namespace KetaEngine {

class EasingSequence {
public:
    EasingSequence()  = default;
    ~EasingSequence() = default;

    /// <summary>
    /// ステップをJSONファイルから追加
    /// </summary>
    /// <param name="name">JSONファイル名</param>
    /// <param name="adaptValue">適用する値へのポインタ</param>
    template <typename T>
    void AddStep(const std::string& name, T* adaptValue);

    /// <summary>
    /// イージングオブジェクトからステップを追加
    /// </summary>
    /// <param name="easing">イージングオブジェクト</param>
    template <typename T>
    void AddStep(std::unique_ptr<Easing<T>> easing);

    /// <summary>
    /// シーケンスの更新
    /// </summary>
    /// <param name="deltaTime">デルタタイム</param>
    void Update(float deltaTime);

    /// <summary>
    /// 全ステップに基準値を設定
    /// </summary>
    /// <param name="value">基準値</param>
    template <typename T>
    void SetBaseValue(const T& value);

    const IEasingStep* GetCurrentStep() const;

    void Reset(); //< シーケンスをリセット
    bool IsAllFinished() const; //< 全ステップが終了したか
    bool IsAllPlaying() const; //< いずれかのステップが再生中か

private:
    std::vector<std::unique_ptr<IEasingStep>> steps_;
    std::function<void()> onAllFinishCallback_ = nullptr;
    size_t currentStep_                        = 0;
    bool loop_                                 = false;

public:
    const size_t& GetCurrentIndex() const { return currentStep_; }
    const size_t& GetStepCount() const { return steps_.size(); }
    void SetLoop(bool loop) { loop_ = loop; }
    void SetOnAllFinishCallback(std::function<void()> callBack) { onAllFinishCallback_ = callBack; }
};

}; // KetaEngine
