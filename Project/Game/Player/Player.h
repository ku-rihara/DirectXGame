#pragma once
#include "3d/ViewProjection.h"
#include "BaseObject/BaseObject.h"
#include "Collider/AABBCollider.h"
#include "CollisionBox/PlayerCollisionInfo.h"
#include "ComboAttackBehavior/BaseComboAttackBehavior.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "Editor/ParticleEditor/ParticleEmitter.h"
#include "Effect/PlayerEffects.h"
#include "JumpAttackUI/JumpAttackUI.h"
#include "Parameter/PlayerParameter.h"
#include "Parts/PlayerHandLeft.h"
#include "Parts/PlayerHandRight.h"
#include "PlayerBehavior/BasePlayerBehavior.h"
#include "TitleBehavior/BaseTitleBehavior.h"
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
    void TitleUpdate(); //< タイトル更新
    void GameIntroUpdate(); //< ゲームイントロ更新

    /// <summary>
    /// 影フラグ設定
    /// </summary>
    /// <param name="isShadow">影の有効/無効</param>
    void SetShadowFrag(const bool& isShadow);

    /// <summary>
    /// 移動
    /// </summary>
    /// <param name="speed">移動速度</param>
    void Move(float speed);

    bool CheckIsMoving(); //< 移動中判定
    void MoveToLimit(); //< 移動制限
    Vector3 GetInputDirection(); //< 入力方向取得
    void UpdateMatrix(); //< 行列更新

    // reset
    void ResetPositionY(); //< Y座標リセット
    void ResetHeadScale(); //< 頭スケールリセット

    /// <summary>
    /// ディゾルブ更新
    /// </summary>
    /// <param name="dissolve">ディゾルブ値</param>
    void DissolveUpdate(float dissolve);

    /// <summary>
    /// ジャンプ
    /// </summary>
    /// <param name="speed">速度</param>
    /// <param name="fallSpeedLimit">落下速度制限</param>
    /// <param name="gravity">重力</param>
    void Jump(float& speed, float fallSpeedLimit, float gravity);

    /// <summary>
    /// 落下
    /// </summary>
    /// <param name="speed">速度</param>
    /// <param name="fallSpeedLimit">落下速度制限</param>
    /// <param name="gravity">重力</param>
    /// <param name="isJump">ジャンプ中か</param>
    void Fall(float& speed, float fallSpeedLimit, float gravity, const bool& isJump = false);

    // 各Behavior切り替え処理
    void ChangeBehavior(std::unique_ptr<BasePlayerBehavior> behavior);
    void ChangeComboBehavior(std::unique_ptr<BaseComboAttackBehavior> behavior);
    void ChangeTitleBehavior(std::unique_ptr<BaseTitleBehavior> behavior);

    /// <summary>
    /// 衝突継続時の処理
    /// </summary>
    /// <param name="other">衝突相手</param>
    void OnCollisionStay([[maybe_unused]] BaseCollider* other) override;

    void ChangeCombBoRoot(); //< コンボルート変更
    void FaceToTarget(); //< ターゲット方向を向く
    void AdaptRotate(); //< 回転適用
    bool CheckIsChargeMax() const; //< チャージ最大判定
    void AdjustParam(); //< パラメータ調整
    Vector3 GetCollisionPos() const override; //< 衝突位置取得

    void MainHeadAnimationStart(const std::string& name);

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

    std::unique_ptr<PlayerHandLeft> leftHand_;
    std::unique_ptr<PlayerHandRight> rightHand_;
    std::unique_ptr<PlayerEffects> effects_;
    std::unique_ptr<PlayerParameter> parameters_;
    std::unique_ptr<PlayerCollisionInfo> playerCollisionInfo_;
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
    PlayerEffects* GetEffects() const { return effects_.get(); }
    LockOnController* GetLockOn() const { return pLockOn_; }
    GameCamera* GetGameCamera() const { return pGameCamera_; }
    AttackEffect* GetAttackEffect() const { return pAttackEffect_; }
    PlayerParameter* GetParameter() const { return parameters_.get(); }
    PlayerCollisionInfo* GetPlayerCollisionInfo() const { return playerCollisionInfo_.get(); }
    PlayerComboAttackController* GetComboAttackController() const { return comboAttackController_; }
    JumpAttackUI* GetJumpAttackUI() const { return jumpAttackUI_.get(); }
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