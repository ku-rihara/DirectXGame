#pragma once
#include "Editor/DissolveEditor/DissolvePlayer.h"
#include "Editor/ObjEaseAnimation/ObjEaseAnimationPlayer.h"
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

    // -------------------------------------------------------
    // ジャンプアニメーション
    // -------------------------------------------------------
    void PlayJumpAnimation();

    // -------------------------------------------------------
    // ObjEase アニメーション (タイトル等)
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

    // 移動・待機アニメーション共用プレイヤー
    KetaEngine::ObjEaseAnimationPlayer moveAnimationPlayer_;

    KetaEngine::Object3d*       obj3d_         = nullptr;
    PlayerHandLeft*             leftHand_       = nullptr;
    PlayerHandRight*            rightHand_      = nullptr;
    KetaEngine::WorldTransform* baseTransform_  = nullptr;
};
