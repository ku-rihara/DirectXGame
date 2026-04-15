#pragma once
#include "Editor/DissolveEditor/DissolvePlayer.h"
#include "Editor/ObjEaseAnimation/ObjEaseAnimationPlayer.h"
#include "Editor/RibbonTrailEditor/RibbonTrailPlayer.h"
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

    /// 毎フレーム更新
    void Update();

    // -------------------------------------------------------
    // Dissolve
    // -------------------------------------------------------
    void PlayDissolve(const std::string& name);
    bool IsDissolveFinished() const;
    void SetInitialDissolveHidden();

    // -------------------------------------------------------
    // 移動 / 待機アニメーション
    // 状態が切り替わった時だけ PlayerMove から呼ぶ
    // -------------------------------------------------------
    void PlayMoveAnimation();
    void PlayWaitAnimation();
    void StopMoveAnimation();

    // -------------------------------------------------------
    // ジャンプアニメーション
    // -------------------------------------------------------
    void PlayJumpAnimation();

    // -------------------------------------------------------
    // ダッシュアニメーション
    // -------------------------------------------------------
    void PlayDashStartAnimation();
    bool IsDashStartAnimationFinished() const;

    // -------------------------------------------------------
    // ObjEase アニメーション (タイトル等)
    // -------------------------------------------------------
    void PlayMainHeadAnimation(const std::string& name);
    void StartMainHeadTrailEmit(const std::string& presetName, const std::string& category = "Player");
    void StopMainHeadTrailEmit();
    void PlayTitleBodyAnimation(const std::string& name);
    void PlayTitleRightHandAnimation(const std::string& name);
    void PlayTitleLeftHandAnimation(const std::string& name);

    bool IsTitleBodyAnimationFinished() const;
    bool IsTitleRightHandAnimationFinished() const;
    bool IsTitleLeftHandAnimationFinished() const;

private:
    KetaEngine::DissolvePlayer   dissolvePlayer_;
    KetaEngine::RibbonTrailPlayer mainHeadTrailPlayer_;
    Vector3                      headTrailFollowPos_{};

    KetaEngine::Object3d*       obj3d_         = nullptr;
    PlayerHandLeft*             leftHand_       = nullptr;
    PlayerHandRight*            rightHand_      = nullptr;
    KetaEngine::WorldTransform* baseTransform_  = nullptr;
};
