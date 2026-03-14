#pragma once
#include "Editor/DissolveEditor/DissolvePlayer.h"
#include <string>

namespace KetaEngine {
class Object3d;
class WorldTransform;
} // namespace KetaEngine

class PlayerHandLeft;
class PlayerHandRight;

/// <summary>
/// プレイヤーアニメーション管理クラス
/// ディゾルブ・ObjEaseアニメーションの再生/判定を一元管理
/// </summary>
class PlayerAnimator {
public:
    void Init(
        KetaEngine::Object3d* obj3d,
        PlayerHandLeft* leftHand,
        PlayerHandRight* rightHand,
        KetaEngine::WorldTransform* baseTransform);

    /// 毎フレーム更新 (ディゾルブ更新・適用)
    void Update();

    // -------------------------------------------------------
    // Dissolve
    // -------------------------------------------------------
    void PlayDissolve(const std::string& name);
    bool IsDissolveFinished() const;
    void SetInitialDissolveHidden();

    // -------------------------------------------------------
    // ObjEase アニメーション
    // -------------------------------------------------------
    void PlayMainHeadAnimation(const std::string& name);
    void PlayTitleBodyAnimation(const std::string& name);
    void PlayTitleRightHandAnimation(const std::string& name);
    void PlayTitleLeftHandAnimation(const std::string& name);

    bool IsTitleBodyAnimationFinished() const;
    bool IsTitleRightHandAnimationFinished() const;
    bool IsTitleLeftHandAnimationFinished() const;

private:
    KetaEngine::DissolvePlayer dissolvePlayer_;

    KetaEngine::Object3d*      obj3d_         = nullptr;
    PlayerHandLeft*            leftHand_       = nullptr;
    PlayerHandRight*           rightHand_      = nullptr;
    KetaEngine::WorldTransform* baseTransform_ = nullptr;
};
