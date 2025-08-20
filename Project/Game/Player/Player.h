#pragma once

#include "3d/ViewProjection.h"

// object
#include "BaseObject/BaseObject.h"
#include "Effect/PlayerEffects.h"
#include "Parameter/PlayerParameter.h"
#include"CollisionBox/PlayerAttackController.h"

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
class Combo;
class Player : public BaseObject, public AABBCollider {

private: // struct
    struct ParticleEffect {
        std::string name;
        std::unique_ptr<ParticleEmitter> emitter;
    };

public:
    Player()  = default;
    ~Player() = default;
    /// ===================================================
    ///  public method
    /// ===================================================

    // 初期化、更新、描画
    void Init() override;
    void Update() override;
    void TitleUpdate();
  
    // 移動
    void Move(const float& speed); // 移動
    bool CheckIsMoving(); // 動かしてるかフラグ
    void MoveToLimit(); // 移動制限
    Vector3 GetInputDirection(); // 入力による速度
    void UpdateMatrix(); // 行列更新

    void PositionYReset();
    void AttackPowerCharge();

    void DissolveUpdate(const float& dissolve);

    // ジャンプ
    void Jump(float& speed, const float& fallSpeedLimit, const float& gravity); /// ジャンプ
    void Fall(float& speed, const float& fallSpeedLimit, const float& gravity, const bool& isJump = false); /// 落ちる

    // 振る舞い切り替え
    void ChangeBehavior(std::unique_ptr<BasePlayerBehavior> behavior);
    void ChangeComboBehavior(std::unique_ptr<BaseComboAattackBehavior> behavior);
    void ChangeTitleBehavior(std::unique_ptr<BaseTitleBehavior> behavior);
    void ChangeCombBoRoot();

    // 向き
    void FaceToTarget();
    void AdaptRotate();

    // chargeCheck
    bool CheckIsChargeMax() const;

    // ParamEdit
    void AdjustParam();

    // collision
    void OnCollisionStay([[maybe_unused]] BaseCollider* other) override;
    Vector3 GetCollisionPos() const override;

  
    // sound
    void SoundPunch();
    void SoundStrongPunch();
    void FallSound();

private: // class
    GlobalParameter* globalParameter_;
    const std::string groupName_ = "Player";

    /// other class
    LockOn* pLockOn_         = nullptr;
    GameCamera* pGameCamera_ = nullptr;
    Combo* pCombo_           = nullptr;

    const ViewProjection* viewProjection_ = nullptr;

    std::unique_ptr<PlayerHandLeft> leftHand_;
    std::unique_ptr<PlayerHandRight> rightHand_;
    std::unique_ptr<PlayerEffects> effects_;
    std::unique_ptr<PlayerParameter> parameters_;
    std::unique_ptr<PlayerAttackController> attackController_;
    
    /// behavior
    std::unique_ptr<BasePlayerBehavior> behavior_            = nullptr;
    std::unique_ptr<BaseComboAattackBehavior> comboBehavior_ = nullptr;
    std::unique_ptr<BaseTitleBehavior> titleBehavior_        = nullptr;

private:
    /// ===================================================
    /// private variables
    /// ===================================================

    // move
    float objectiveAngle_; /// 目標角度
    Vector3 direction_; /// 速度
    Vector3 prePos_; /// 移動前座標
    float moveSpeed_; /// 移動速度

    // attackCharge
    float currentUpperChargeTime_;

    // sound
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
    PlayerAttackController* GetAttackController() const { return attackController_.get(); }
    float GetMoveSpeed() const { return moveSpeed_; }

    /// =========================================================================================
    /// setter
    /// =========================================================================================
    void SetViewProjection(const ViewProjection* viewProjection) { viewProjection_ = viewProjection; }
    void SetLockOn(LockOn* lockon);
    void SetGameCamera(GameCamera* gamecamera);
    void SetCombo(Combo* combo);
    void SetTitleBehavior();
    void RotateReset();
    void HeadLightSetting();

    void SetHeadScale(const Vector3& scale) { obj3d_->transform_.scale_ = scale; }
    void SetHeadPosY(const float& posy) { obj3d_->transform_.translation_.y = posy; }
    void SetHeadRotateX(const float& zrotate) { obj3d_->transform_.rotation_.x = zrotate; }
    void SetHeadRotateY(const float& zrotate) { obj3d_->transform_.rotation_.y = zrotate; }
};