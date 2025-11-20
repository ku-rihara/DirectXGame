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

    // 初期化、更新
    void Init();
    void Update();

    // 再生、停止
    void Play(const std::string& categoryName, const std::string& animationName);
    void Stop();

private:
    std::unique_ptr<ObjEaseAnimationData> animationData_;
    std::string currentCategoryName_;
    std::string currentAnimationName_;

public:
    bool IsPlaying() const { return animationData_->IsPlaying(); }
    bool IsFinished() const { return animationData_->IsFinished(); }

    Vector3 GetCurrentScale() const;
    Vector3 GetCurrentRotation() const;
    Vector3 GetCurrentTranslation() const;

    ObjEaseAnimationData* GetAnimationData() { return animationData_.get(); }
};