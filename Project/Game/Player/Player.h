#pragma once
#include "3d/ViewProjection.h"
#include "BaseObject/BaseObject.h"
#include "Collider/AABBCollider.h"
#include "CollisionBox/PlayerAttackController.h"
#include "ComboAttackBehavior/BaseComboAattackBehavior.h"
#include "Effect/PlayerEffects.h"
#include "JumpAttackUI/JumpAttackUI.h"
#include "Parameter/PlayerParameter.h"
#include "Parts/PlayerHandLeft.h"
#include "Parts/PlayerHandRight.h"
#include "PlayerBehavior/BasePlayerBehavior.h"
#include "TitleBehavior/BaseTitleBehavior.h"
#include "utility/ParameterEditor/GlobalParameter.h"
#include "utility/ParticleEditor/ParticleEmitter.h"
#include <memory>
#include <string>

class SpecialAttackGauge;
class LockOnController;
class GameCamera;
class Combo;
class AttackEffect;

/// <summary>
/// プレイヤークラス
/// </summary>
class Player : public BaseObject, public AABBCollider {
private:
    struct ParticleEffect {
        std::string name;
        std::unique_ptr<ParticleEmitter> emitter;
    };

public:
    Player()  = default;
    ~Player() = default;

    // 初期化、更新
    void Init() override;
    void Update() override;

    void GameSceneInit(); //< ゲームシーン初期化
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
    void Move(const float& speed);

    bool CheckIsMoving(); //< 移動中判定
    void MoveToLimit(); //< 移動制限
    Vector3 GetInputDirection(); //< 入力方向取得
    void UpdateMatrix(); //< 行列更新
    void PositionYReset(); //< Y座標リセット
    void AttackPowerCharge(); //< 攻撃力チャージ

    /// <summary>
    /// ディゾルブ更新
    /// </summary>
    /// <param name="dissolve">ディゾルブ値</param>
    void DissolveUpdate(const float& dissolve);

    /// <summary>
    /// ジャンプ
    /// </summary>
    /// <param name="speed">速度</param>
    /// <param name="fallSpeedLimit">落下速度制限</param>
    /// <param name="gravity">重力</param>
    void Jump(float& speed, const float& fallSpeedLimit, const float& gravity);

    /// <summary>
    /// 落下
    /// </summary>
    /// <param name="speed">速度</param>
    /// <param name="fallSpeedLimit">落下速度制限</param>
    /// <param name="gravity">重力</param>
    /// <param name="isJump">ジャンプ中か</param>
    void Fall(float& speed, const float& fallSpeedLimit, const float& gravity, const bool& isJump = false);

    /// <summary>
    /// 挙動切り替え
    /// </summary>
    /// <param name="behavior">新しい挙動</param>
    void ChangeBehavior(std::unique_ptr<BasePlayerBehavior> behavior);

    /// <summary>
    /// コンボ挙動切り替え
    /// </summary>
    /// <param name="behavior">新しいコンボ挙動</param>
    void ChangeComboBehavior(std::unique_ptr<BaseComboAattackBehavior> behavior);

    /// <summary>
    /// タイトル挙動切り替え
    /// </summary>
    /// <param name="behavior">新しいタイトル挙動</param>
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

    void SoundPunch(); //< パンチ音再生
    void SoundStrongPunch(); //< 強パンチ音再生
    void FallSound(); //< 落下音再生

private: 
    GlobalParameter* globalParameter_;
    const std::string groupName_ = "Player";

    /// other class
    LockOnController* pLockOn_ = nullptr;
    GameCamera* pGameCamera_   = nullptr;
    Combo* pCombo_             = nullptr;
    AttackEffect* pHitStop_    = nullptr;

    const ViewProjection* viewProjection_ = nullptr;

    std::unique_ptr<PlayerHandLeft> leftHand_;
    std::unique_ptr<PlayerHandRight> rightHand_;
    std::unique_ptr<PlayerEffects> effects_;
    std::unique_ptr<PlayerParameter> parameters_;
    std::unique_ptr<PlayerAttackController> attackController_;
    std::unique_ptr<JumpAttackUI> jumpAttackUI_;

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
    // getter
    PlayerHandLeft* GetLeftHand() const { return leftHand_.get(); }
    PlayerHandRight* GetRightHand() const { return rightHand_.get(); }
    BasePlayerBehavior* GetBehavior() const { return behavior_.get(); }
    BaseTitleBehavior* GetTitleBehavior() const { return titleBehavior_.get(); }
    PlayerEffects* GetEffects() const { return effects_.get(); }
    LockOnController* GetLockOn() const { return pLockOn_; }
    GameCamera* GetGameCamera() const { return pGameCamera_; }
    AttackEffect* GetAttackEffect() const { return pHitStop_; }
    PlayerParameter* GetParameter() const { return parameters_.get(); }
    PlayerAttackController* GetAttackController() const { return attackController_.get(); }
    JumpAttackUI* GetJumpAttackUI() const { return jumpAttackUI_.get(); }
    const float& GetMoveSpeed() const { return moveSpeed_; }

    // setter
    void SetViewProjection(const ViewProjection* viewProjection) { viewProjection_ = viewProjection; }
    void SetLockOn(LockOnController* lockon);
    void SetGameCamera(GameCamera* gamecamera);
    void SetCombo(Combo* combo);
    void SetHitStop(AttackEffect* hitStop);
    void SetTitleBehavior();
    void RotateReset();
    void HeadLightSetting();
    void SetHeadScale(const Vector3& scale) { obj3d_->transform_.scale_ = scale; }
    void SetHeadPosY(const float& posy) { obj3d_->transform_.translation_.y = posy; }
    void SetHeadRotateX(const float& zRotate) { obj3d_->transform_.rotation_.x = zRotate; }
    void SetHeadRotateY(const float& zRotate) { obj3d_->transform_.rotation_.y = zRotate; }
};