#pragma once
#include "3D/AnimationObject3D/Object3DAnimation.h"
#include <array>
#include <memory>
#include <string>
#include <vector>

/// <summary>
/// 敵のアニメーション管理クラス
/// </summary>
class EnemyAnimator {
public:
    struct DamageReactionAnimInfo {
        std::string name;
        bool isLoop = false;
    };

    enum class AnimationType {
        Wait,
        Spawn,
        Discovery,
        Dash,
        DamageReaction,
        Death,
        Taunt,
        Count
    };

    enum class ChaseAnimationState {
        NONE,
        PRE_DASH,
        DASHING
    };

public:
    /// <summary>
    /// モデルフォルダを設定しランタイム状態をリセット
    /// </summary>
    void Init(const std::string& modelFolder);

    void AddDamageReactionAnimation(const std::string& name, bool isLoop = false);
    /// <summary>
    /// モデルフォルダ付きで .gltf を追加
    /// </summary>
    void AddAnimationFile(const std::string& relativeName);

    // アニメーション再生
    void PlayAnimation(AnimationType type, bool isLoop = false);
    bool PlayAnimationByName(const std::string& animationName, bool isLoop = false);

    void UpdateChaseAnimation(float deltaTime);
    void ResetToWaitAnimation();
    void SetAnimationActive(bool active);
    void RotateInit();
    void ClearAllCallbacks();

private:
    std::string modelFolder_;
    std::unique_ptr<KetaEngine::Object3DAnimation> objAnimation_;
    std::array<std::string, static_cast<size_t>(AnimationType::Count)> animationNames_{};
    std::vector<DamageReactionAnimInfo> damageReactionAnimations_;
    ChaseAnimationState chaseAnimeState_ = ChaseAnimationState::NONE;
    bool isPreDashFinished_              = false;

public:
    // getter
    Vector3 GetBodyRotation() const;
    KetaEngine::Object3DAnimation* GetAnimationObject() const { return objAnimation_.get(); }
    std::vector<std::string> GetAnimationNames() const;
    const std::string& GetAnimationName(AnimationType type) const;
    std::vector<std::string> GetDamageReactionAnimationNames() const;
    bool GetDamageReactionAnimationIsLoop(const std::string& name) const;
    ChaseAnimationState GetChaseAnimState() const { return chaseAnimeState_; }
    bool IsPreDashFinished() const { return isPreDashFinished_; }
    const std::string& GetModelFolder() const { return modelFolder_; }

    // Setter
    void SetBodyRotate(const Vector3& rotate);
    void SetBodyColor(const Vector4& color);
    void SetAnimationName(AnimationType type, const std::string& name);
};
