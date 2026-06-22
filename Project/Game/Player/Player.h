#pragma once
// 3D
#include "3D/Object3D/Object3d.h"
#include "3D/ViewProjection.h"
// Base
#include "BaseObject/BaseObject.h"
// Collider
#include "Collider/AABBCollider.h"
#include "Components/CollisionBox/PlayerAttackCollider.h"
// BehaviorManager
#include "Behavior/PlayerBehaviorManager.h"
// Editor
#include "Editor/ParameterEditor/GlobalParameter.h"
// Animation
#include "Components/Animation/PlayerAnimator.h"
// Effect
#include "Components/Effect/PlayerEffects.h"
// Parameter
#include "Components/Parameter/PlayerParameter.h"
// Input
#include "Components/Input/PlayerInput.h"
// Parts
#include "Components/Parts/PlayerHandLeft.h"
#include "Components/Parts/PlayerHandRight.h"
// ComboCreator
#include "Player/AutoComboAttack/AutoComboQueue.h"
// Context
// std
#include <functional>
#include <memory>
#include <string>

class LockOnController;
class GameCamera;
class Combo;
class PlayerComboAttackController;
class StressGauge;

/// <summary>
/// プレイヤークラス
/// </summary>
class Player : public BaseObject, public KetaEngine::AABBCollider {
private:
    struct PlayerContext {
        LockOnController* pLockOn                          = nullptr;
        GameCamera* pGameCamera                            = nullptr;
        Combo* pCombo                                      = nullptr;
        PlayerComboAttackController* comboAttackController = nullptr;
        StressGauge* pStressGauge                            = nullptr;
    };

public:
    Player()  = default;
    ~Player() = default;

    // 初期化処理
    void Init() override;
    void InitInGameScene();

    // 更新処理
    void Update() override;
    void GameIntroUpdate(float playSpeed = 1.0f);

    /// <summary>
    /// 移動
    /// </summary>
    /// <param name="speed">移動スピード</param>
    void Move(float speed);
    void MoveToLimit(); //< 移動範囲制限
    void UpdateMatrix(); //< 行列更新

    /// <summary>
    /// ジャンプの動き
    /// </summary>
    /// <param name="speed">ジャンプスピード</param>
    /// <param name="fallSpeedLimit">落下速度上限</param>
    /// <param name="gravity">重力</param>
    void Jump(float& speed, float fallSpeedLimit, float gravity);

    /// <summary>
    /// ジャンプから落ちる処理
    /// </summary>
    /// <param name="speed">落下速度</param>
    /// <param name="fallSpeedLimit">落下速度上限</param>
    /// <param name="gravity">重力</param>
    /// <param name="isJump">ジャンプによる落下か</param>
    void Fall(float& speed, float fallSpeedLimit, float gravity, bool isJump = false);

    // 衝突コールバック
    void OnCollisionStay([[maybe_unused]] BaseCollider* other) override;

    /// <summary>
    /// 自動実行コールバックを登録
    /// </summary>
    void SetAutoComboAttackCallback(std::function<void(const std::string&)> callback) {
        autoComboAttackCallback_ = std::move(callback);
    }

    /// <summary>
    /// 自動実行攻撃発火を通知
    /// </summary>
    void FireAutoComboAttackCallback(const std::string& attackName) {
        if (autoComboAttackCallback_) {
            autoComboAttackCallback_(attackName);
        }
    }

    void ChangeCombBoRoot(); //< コンボルート変更
    void FaceToTarget(); //< ターゲット方向を向く
    void AdaptRotate(); //< 回転適用
    bool CheckIsChargeMax() const; //< チャージ最大判定
    void AdjustParam(); //< パラメータ調整
    Vector3 GetCollisionPos() const override; //< 衝突位置取得

    /// 自動ダッシュ開始、解除
    void StartAutoDash();
    void ClearAutoDash();

    void HeadLightSetting();

    // Behavior切り替え
    void ChangeBehavior(std::unique_ptr<BasePlayerBehavior> behavior) { behaviors_.Change(std::move(behavior)); }
    void ChangeCombo(std::unique_ptr<BaseComboAttackBehavior> combo) { behaviors_.ChangeCombo(std::move(combo)); }

private:
    void ChangeDeathMode();
    bool IsAbleBehavior();

protected:
    PlayerBehaviorManager behaviors_;
    PlayerEffects effects_;

private:
    // グローバルパラメータ
    KetaEngine::GlobalParameter* globalParameter_;
    const std::string groupName_ = "Player";

    // ViewProjectionの取得
    const KetaEngine::ViewProjection* viewProjection_ = nullptr;

    PlayerAnimator animator_;
    PlayerContext context_;
    AutoComboQueue autoComboQueue_;
    std::unique_ptr<KetaEngine::Object3d> obj3d_;

    PlayerHandLeft leftHand_;
    PlayerHandRight rightHand_;
    PlayerParameter parameters_;
    PlayerAttackCollider playerCollisionInfo_;

    // 自動実行攻撃時コールバック
    std::function<void(const std::string&)> autoComboAttackCallback_;

private:
    /// ===================================================
    /// private variables
    /// ===================================================

    // input
    PlayerInput input_;

    // move
    float objectiveAngle_;
    float objectiveAnglePitch_ = 0.0f;
    Vector3 direction_;
    Vector3 prePos_;
    float moveSpeed_;

    // attackCharge
    float currentUpperChargeTime_;

    // 死亡フラグ
    const bool* isDeath_;
    bool isDeathRenditionFinish_ = false;

    // コンボアンロックスキップ
    bool isIgnoreUnlockState_ = false;

public:
    /// ===================================================
    /// Getter Method
    /// ===================================================
    PlayerHandLeft* GetLeftHand() { return &leftHand_; }
    PlayerHandRight* GetRightHand() { return &rightHand_; }
    const PlayerBehaviorManager& GetBehaviors() const { return behaviors_; }
    PlayerEffects* GetEffects() { return &effects_; }
    PlayerParameter* GetParameter() { return &parameters_; }
    PlayerAttackCollider* GetPlayerCollisionInfo() { return &playerCollisionInfo_; }
    const PlayerContext& GetContext() const { return context_; }
    bool GetIsIgnoreUnlockState() const { return isIgnoreUnlockState_; }
    const PlayerInput& GetInput() const { return input_; }
    PlayerAnimator& GetPlayerAnimator() { return animator_; }
    KetaEngine::Object3d* GetObject3D() const { return obj3d_.get(); }
    float GetMoveSpeed() const { return moveSpeed_; }
    bool GetIsDeathRenditionFinish() const { return isDeathRenditionFinish_; }
    AutoComboQueue& GetAutoComboQueue() { return autoComboQueue_; }

    /// ===================================================
    /// Setter Method
    /// ===================================================
    void SetViewProjection(const KetaEngine::ViewProjection* viewProjection);
    void SetStressGauge(StressGauge* StressGauge);
    void SetDeathFragPointer(const bool* isDeath) { isDeath_ = isDeath; }
    void SetIsDeathRenditionFinish(bool isFinish) { isDeathRenditionFinish_ = isFinish; }
    void SetIsIgnoreUnlockState(bool isIgnore) { isIgnoreUnlockState_ = isIgnore; }
    void SetObjectiveAngle(float angle) { objectiveAngle_ = angle; }
    void SetObjectiveAnglePitch(float angle) { objectiveAnglePitch_ = angle; }
    void SetShadowFrag(bool isShadow);
    // Class Set
    void SetLockOn(LockOnController* lockon);
    void SetGameCamera(GameCamera* gamecamera);
    void SetCombo(Combo* combo);
    void SetComboAttackController(PlayerComboAttackController* playerComboAttackController);
};