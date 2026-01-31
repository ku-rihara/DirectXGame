#pragma once
// 3D
#include "3D/ViewProjection.h"
#include "3d/Object3D/Object3d.h"
// Base
#include "BaseObject/BaseObject.h"
// Collider
#include "Collider/AABBCollider.h"
#include "CollisionBox/PlayerAttackCollisionBox.h"
// Behavior
#include "Behavior/ComboAttackBehavior/BaseComboAttackBehavior.h"
#include "Behavior/PlayerBehavior/BasePlayerBehavior.h"
#include "Behavior/TitleBehavior/BaseTitleBehavior.h"
// Editor
#include "Editor/ParameterEditor/GlobalParameter.h"
// Particle,Effect
#include "Particle/CPUParticle/Editor/ParticleEmitter.h"
#include "Effect/PlayerEffects.h"
// UI
#include "JumpAttackUI/JumpAttackUI.h"
// Parameter
#include "Parameter/PlayerParameter.h"
// Parts
#include "Parts/PlayerHandLeft.h"
#include "Parts/PlayerHandRight.h"
// std
#include <memory>
#include <string>

class SpecialAttackGauge;
class LockOnController;
class GameCamera;
class Combo;
class AttackEffect;
class PlayerComboAttackController;

/// <summary>
/// プレイヤークラス
/// </summary>
class Player : public BaseObject, public KetaEngine::AABBCollider {
private:
    struct ParticleEffect {
        std::string name;
        std::unique_ptr<KetaEngine::ParticleEmitter> emitter;
    };

public:
    Player()  = default;
    ~Player() = default;

    // 初期化、更新
    void Init() override;
    void Update() override;

    void InitInGameScene(); //< ゲームシーンでの初期化
    void TitleUpdate();     //< タイトル更新処理
    void GameIntroUpdate(); //< イントロシーン更新

    // 影適応するかのフラグセット
    void SetShadowFrag(const bool& isShadow);

    /// <summary>
    /// 移動
    /// </summary>
    /// <param name="speed">移動スピード</param>
    void Move(float speed);

    bool CheckIsMoving();        //< 移動中かチェック
    void MoveToLimit();          //< 移動範囲制限
    Vector3 GetInputDirection(); //< 入力による方向決定
    void UpdateMatrix();         //< 行列更新

    // reset
    void ResetPositionY(); //< Y座標リセット
    void ResetHeadScale(); //< 頭のスケールリセット

    /// <summary>
    /// ディゾルブ処理更新
    /// </summary>
    /// <param name="dissolve">Dissolve</param>
    void DissolveUpdate(float dissolve);

    /// <summary>
    /// ジャンプ
    /// </summary>
    /// <param name="speed">ジャンプスピード</param>
    /// <param name="fallSpeedLimit">落下速度上限</param>
    /// <param name="gravity">重力</param>
    void Jump(float& speed, float fallSpeedLimit, float gravity);

    /// <summary>
    /// 落ちる
    /// </summary>
    /// <param name="speed">落下速度</param>
    /// <param name="fallSpeedLimit">落下速度上限</param>
    /// <param name="gravity">重力</param>
    /// <param name="isJump">ジャンプによる落下か</param>
    void Fall(float& speed, float fallSpeedLimit, float gravity, const bool& isJump = false);

    // Behavior
    void ChangeBehavior(std::unique_ptr<BasePlayerBehavior> behavior);
    void ChangeComboBehavior(std::unique_ptr<BaseComboAttackBehavior> behavior);
    void ChangeTitleBehavior(std::unique_ptr<BaseTitleBehavior> behavior);

    // 衝突コールバック
    void OnCollisionStay([[maybe_unused]] BaseCollider* other) override;

    void ChangeCombBoRoot();                  //< コンボルート変更
    void FaceToTarget();                      //< ターゲット方向を向く
    void AdaptRotate();                       //< 回転適用
    bool CheckIsChargeMax() const;            //< チャージ最大判定
    void AdjustParam();                       //< パラメータ調整
    Vector3 GetCollisionPos() const override; //< 衝突位置取得

    void MainHeadAnimationStart(const std::string& name);

    /// <summary>
    /// ダッシュ中かどうかを取得
    /// </summary>
    bool IsDashing() const;

private:
    void ChangeDeathMode();
    bool IsAbleBehavior();

private:
    KetaEngine::GlobalParameter* globalParameter_;
    const std::string groupName_ = "Player";

    /// other class
    LockOnController* pLockOn_                          = nullptr;
    GameCamera* pGameCamera_                            = nullptr;
    Combo* pCombo_                                      = nullptr;
    AttackEffect* pAttackEffect_                        = nullptr;
    PlayerComboAttackController* comboAttackController_ = nullptr;

    const KetaEngine::ViewProjection* viewProjection_ = nullptr;

    std::unique_ptr<KetaEngine::Object3d> obj3d_;
    std::unique_ptr<PlayerHandLeft> leftHand_;
    std::unique_ptr<PlayerHandRight> rightHand_;
    std::unique_ptr<PlayerEffects> effects_;
    std::unique_ptr<PlayerParameter> parameters_;
    std::unique_ptr<PlayerAttackCollisionBox> playerCollisionInfo_;
    std::unique_ptr<JumpAttackUI> jumpAttackUI_;

    /// behavior
    std::unique_ptr<BasePlayerBehavior> behavior_           = nullptr;
    std::unique_ptr<BaseComboAttackBehavior> comboBehavior_ = nullptr;
    std::unique_ptr<BaseTitleBehavior> titleBehavior_       = nullptr;

private:
    /// ===================================================
    /// private variables
    /// ===================================================

    // move
    float objectiveAngle_;
    Vector3 direction_;
    Vector3 prePos_;
    float moveSpeed_;

    // attackCharge
    float currentUpperChargeTime_;

    //
    const bool* isDeath_;
    bool isDeathRenditionFinish_ = false;

public:
    // getter
    PlayerHandLeft* GetLeftHand() const { return leftHand_.get(); }
    PlayerHandRight* GetRightHand() const { return rightHand_.get(); }
    BasePlayerBehavior* GetBehavior() const { return behavior_.get(); }
    BaseTitleBehavior* GetTitleBehavior() const { return titleBehavior_.get(); }
    BaseComboAttackBehavior* GetComboBehavior() const { return comboBehavior_.get(); }
    PlayerEffects* GetEffects() const { return effects_.get(); }
    LockOnController* GetLockOn() const { return pLockOn_; }
    GameCamera* GetGameCamera() const { return pGameCamera_; }
    AttackEffect* GetAttackEffect() const { return pAttackEffect_; }
    PlayerParameter* GetParameter() const { return parameters_.get(); }
    PlayerAttackCollisionBox* GetPlayerCollisionInfo() const { return playerCollisionInfo_.get(); }
    PlayerComboAttackController* GetComboAttackController() const { return comboAttackController_; }
    JumpAttackUI* GetJumpAttackUI() const { return jumpAttackUI_.get(); }
    KetaEngine::Object3d* GetObject3D() const { return obj3d_.get(); }
    float GetMoveSpeed() const { return moveSpeed_; }
    bool GetIsDeathRenditionFinish() const { return *isDeath_; }

    //*-- setter --*//
    // class Set
    void SetViewProjection(const KetaEngine::ViewProjection* viewProjection);
    void SetLockOn(LockOnController* lockon);
    void SetGameCamera(GameCamera* gamecamera);
    void SetCombo(Combo* combo);
    void SetHitStop(AttackEffect* hitStop);
    void SetComboAttackController(PlayerComboAttackController* playerComboAttackController);
    void SetDeathFragPointer(const bool* isDeath) { isDeath_ = isDeath; }
    void SetIsDeathRenditionFinish(bool isFinish) { isDeathRenditionFinish_ = isFinish; }

    void SetTitleBehavior();
    void RotateReset();
    void HeadLightSetting();
    void SetHeadScale(const Vector3& scale) { obj3d_->transform_.scale_ = scale; }
    void SetHeadPosY(float posy) { obj3d_->transform_.translation_.y = posy; }
    void SetHeadRotateX(float zRotate) { obj3d_->transform_.rotation_.x = zRotate; }
    void SetHeadRotateY(float zRotate) { obj3d_->transform_.rotation_.y = zRotate; }
};