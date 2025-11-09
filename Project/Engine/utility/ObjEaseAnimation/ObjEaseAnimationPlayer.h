#pragma once
#include "ObjEaseAnimationData.h"
#include "Vector3.h"
#include <memory>
#include <string>

/// <summary>
/// オブジェクトイージングアニメーションプレイヤー
/// </summary>
class ObjEaseAnimationPlayer {
public:
    ObjEaseAnimationPlayer()  = default;
    ~ObjEaseAnimationPlayer() = default;

    void Init(); //< 初期化

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="deltaTime">デルタタイム</param>
    void Update(const float& deltaTime);

    /// <summary>
    /// アニメーション再生
    /// </summary>
    /// <param name="categoryName">カテゴリー名</param>
    /// <param name="animationName">アニメーション名</param>
    void Play(const std::string& categoryName, const std::string& animationName);

    void Stop(); //< アニメーション停止

private:
    std::unique_ptr<ObjEaseAnimationData> animationData_;
    std::string currentCategoryName_;
    std::string currentAnimationName_;

public:
    bool IsPlaying() const { return animationData_->IsPlaying(); }
    bool IsFinished() const {return animationData_->IsFinished(); }

    const Vector3& GetCurrentScale() const {return animationData_->GetCurrentScale();}
    const Vector3& GetCurrentRotation() const {return animationData_->GetCurrentRotation();}
    const Vector3& GetCurrentTranslation() const {return animationData_->GetCurrentTranslation();}

    ObjEaseAnimationData* GetAnimationData() { return animationData_.get(); }
};