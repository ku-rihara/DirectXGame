#pragma once

#include "3d/ViewProjection.h"

// object
#include "BaseObject/BaseObject.h"
#include "Effect/PlayerEffects.h"
#include "Parameter/PlayerParameter.h"

/// Parts
#include "Parts/PlayerHandLeft.h"
#include "Parts/PlayerHandRight.h"

/// utility
#include "utility/ParameterEditor/GlobalParameter.h"
#include "utility/ParticleEditor/ParticleEmitter.h"

/// behavior
#include "ComboAttackBehavior/BaseComboAattackBehavior.h"
#include "PlayerBehavior/BasePlayerBehavior.h"
#include "TitleBehavior/BaseTitleBehavior.h"

/// collider
#include "Collider/AABBCollider.h"

/// std
#include <memory>
#include <string>

class SpecialAttackGauge;
class LockOn;
class GameCamera;
class Player : public BaseObject, public AABBCollider {
private:
    struct ParticleEffect {
        std::string name;
        std::unique_ptr<ParticleEmitter> emitter;
    };

public:
    Player();

    /// ===================================================
    ///  public method
    /// ===================================================

    ///* 初期化、更新、描画
    void Init() override;
    void Update() override;
    void TitleUpdate();
    void Draw(const ViewProjection& viewProjection) override;
    void EffectDraw(const ViewProjection& viewProjection);

    ///* 移動
    void Move(const float& speed); /// 移動
    bool GetIsMoving();            /// 動かしてるかフラグ
    void MoveToLimit();            /// 移動制限
    Vector3 GetInputDirection();   /// 入力による速度
    void UpdateMatrix();           /// 行列更新

    void PositionYReset();
    void AttackPowerCharge();

    //* ジャンプ
    void Jump(float& speed, const float& fallSpeedLimit, const float& gravity); /// ジャンプ
    void Fall(float& speed, const float& fallSpeedLimit, const float& gravity, const bool& isJump = false); /// 落ちる

    ///* 振る舞い切り替え
    void ChangeBehavior(std::unique_ptr<BasePlayerBehavior> behavior);
    void ChangeComboBehavior(std::unique_ptr<BaseComboAattackBehavior> behavior);
    void ChangeTitleBehavior(std::unique_ptr<BaseTitleBehavior> behavior);
    void ChangeCombBoRoot();

    ///* 向き
    void FaceToTarget();
    void AdaptRotate();

    bool IsChargeMax() const;

    /// ====================================================================
    /// Editor
    /// ====================================================================

    void AdjustParam();

    /// ====================================================================
    /// Collision
    /// ====================================================================
    void OnCollisionStay([[maybe_unused]] BaseCollider* other) override;
    Vector3 GetCollisionPos() const override;

    /// <summary>
    /// sound
    /// </summary>
    void SoundPunch();
    void SoundStrongPunch();
    void FallSound();

private:
    /// グローバルなパラメータ
    GlobalParameter* globalParameter_; /// グローバルパラメータ
    const std::string groupName_ = "Player"; /// グループ名

    /// behavior
    std::unique_ptr<BasePlayerBehavior> behavior_            = nullptr;
    std::unique_ptr<BaseComboAattackBehavior> comboBehavior_ = nullptr;
    std::unique_ptr<BaseTitleBehavior> titleBehavior_        = nullptr;

    /// Particle
    std::unique_ptr<PlayerEffects> effects_;

    /// parameter
    std::unique_ptr<PlayerParameter> parameters_;

    /// other class
    LockOn* pLockOn_; /// LockOnクラス
    GameCamera* pGameCamera_; /// ゲームカメラポインタ
    std::unique_ptr<PlayerHandLeft> leftHand_; /// 左手
    std::unique_ptr<PlayerHandRight> rightHand_; /// 右手
    std::unique_ptr<Object3d> headObj_; /// 頭
    WorldTransform headTransform_; /// 頭トランスフォーム

private:
    /// ===================================================
    /// private variables
    /// ===================================================

    /// move
    float objectiveAngle_; /// 目標角度
    Vector3 direction_; /// 速度
    Vector3 prePos_; /// 移動前座標
    float moveSpeed_; /// 移動速度

    // attackCharge
    float currentUpperChargeTime_;

    // カメラのビュープロジェクション
    const ViewProjection* viewProjection_ = nullptr;

    /// sound
    int punchSoundID_;
    int strongPunch_;
    int fallSound_;
    int starSound_;

public:
    /// =========================================================================================
    /// getter
    /// =========================================================================================
    PlayerHandLeft* GetLeftHand() const { return leftHand_.get(); }
    PlayerHandRight* GetRightHand() const { return rightHand_.get(); }
    BasePlayerBehavior* GetBehavior() const { return behavior_.get(); }
    BaseTitleBehavior* GetTitleBehavior() const { return titleBehavior_.get(); }
    PlayerEffects* GetEffects() const { return effects_.get(); }
    LockOn* GetLockOn() const { return pLockOn_; }
    GameCamera* GetGameCamera() const { return pGameCamera_; }
    PlayerParameter* GetParameter() const { return parameters_.get(); }
    float GetMoveSpeed() const { return moveSpeed_; }

    /// =========================================================================================
    /// setter
    /// =========================================================================================
    void SetViewProjection(const ViewProjection* viewProjection) { viewProjection_ = viewProjection; }
    void SetLockOn(LockOn* lockon);
    void SetGameCamera(GameCamera* gamecamera);
    void SetTitleBehavior();
    void SetRotateInit();
    void SetLightPos();
    void HeadLightSetting();

    void SetHeadScale(const Vector3& scale) { headTransform_.scale_ = scale; }
    void SetHeadPosY(const float& posy) { headTransform_.translation_.y = posy; }
    void SetHeadRotateX(const float& zrotate) { headTransform_.rotation_.x = zrotate; }
    void SetHeadRotateY(const float& zrotate) { headTransform_.rotation_.y = zrotate; }
};