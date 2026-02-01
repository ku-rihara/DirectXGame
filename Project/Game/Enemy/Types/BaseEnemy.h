#pragma once

// 3D
#include "3d/ViewProjection.h"
// BaseObject
#include "BaseObject/BaseObject.h"
// 3D
#include "3D/AnimationObject3D/Object3DAnimation.h"
// Behavior
#include "../Behavior/ActionBehavior/BaseEnemyBehavior.h"
#include "../Behavior/DamageReactionBehavior/BaseEnemyDamageReaction.h"
// Collision
#include "../CollisionBox/EnemyAttackCollisionBox.h"
#include "Collider/AABBCollider.h"
#include "Enemy/Effects/EnemyEffects.h"
#include "Enemy/HPBar/EnemyHPBar.h"
// AttackStrategy
#include "../Behavior/AttackStrategy/IEnemyAttackStrategy.h"
// std
#include <array>
#include <cstdint>
#include <memory>
#include <vector>

class Player;
class GameCamera;
class EnemyManager;
class Combo;
class PlayerAttackCollisionBox;

/// <summary>
/// 敵の基底クラス
/// </summary>
class BaseEnemy : public BaseObject, public KetaEngine::AABBCollider {
public:
    enum class Type {
        NORMAL,
        STRONG,
        COUNT,
    };

    struct AttackParam {
        float attackValue;
        Vector3 collisionSize;
        Vector3 collisionOffset;
        float adaptTime;
        float attackMoveDistance; // 攻撃時の正面方向への移動量
        float attackMoveSpeed;    // 攻撃時の移動スピード
    };

    struct Parameter {
        AttackParam attackParam;
        Vector3 baseScale_;
        Vector2 hpBarPosOffset;
        float basePosY;
        float burstTime;
        // 追従パラメータ
        float chaseDistance;
        float chaseSpeed;
        float discoveryDelayTime = 0.5f;
        // 攻撃パラメータ
        float attackStartDistance;
        float attackCooldownTime;
        float attackAnticipationTime;
        // 死亡パラメータ
        float deathBlowValue;
        float deathBlowValueY;
        float deathGravity;
        float deathRotateSpeed;
        float deathBurstTime;
    };

    // アニメーションタイプ（今後追加可能）
    enum class AnimationType {
        Wait,           // 待機
        Spawn,          // スポーン
        PreDash,        // ダッシュ予備動作
        Dash,           // ダッシュ
        Attack,         // 攻撃
        DamageReaction, // ダメージリアクション
        Death,          // 死亡
        // 今後追加するアニメーションはここに追加
        Count           // 総数（配列サイズ用）
    };

    // 追跡アニメーション状態
    enum class ChaseAnimationState {
        NONE,      // アニメーションなし
        PRE_DASH,  // ダッシュ予備動作
        DASHING    // ダッシュ中
    };

public:
    BaseEnemy()          = default;
    virtual ~BaseEnemy() = default;

    ///========================================================================================
    ///  public method
    ///========================================================================================

    // 初期化、更新
    virtual void Init(const Vector3& spownPos);
    virtual void Update();

    // 振る舞い個別処理
    virtual void SpawnRenditionInit() = 0;

    /// <summary>
    /// 攻撃戦略を設定
    /// </summary>
    void SetAttackStrategy(std::unique_ptr<IEnemyAttackStrategy> strategy) {
        attackStrategy_ = std::move(strategy);
    }

    /// <summary>
    /// 攻撃戦略を取得
    /// </summary>
    IEnemyAttackStrategy* GetAttackStrategy() const { return attackStrategy_.get(); }

    /// <summary>
    /// スプライトUIの表示
    /// </summary>
    /// <param name="viewProjection">ビュープロジェクション</param>
    virtual void DisplaySprite(const KetaEngine::ViewProjection& viewProjection);

    void ThrustRenditionInit(); //< 突き飛ばし演出初期化
    void DeathRenditionInit(); //< 死亡演出初期化
    void ScaleReset(); //< スケールリセット
    void RotateInit(); //< 回転初期化

    /// <summary>
    /// スポーンアニメーションを再生
    /// </summary>
    void PlaySpawnAnimation();

    /// <summary>
    /// 追跡開始時のアニメーション初期化
    /// </summary>
    void InitChaseAnimation();

    /// <summary>
    /// 追跡中のアニメーション更新
    /// </summary>
    /// <param name="deltaTime">デルタタイム</param>
    void UpdateChaseAnimation(float deltaTime);

    /// <summary>
    /// 待機アニメーションにリセット
    /// </summary>
    void ResetToWaitAnimation();

    /// <summary>
    /// 指定したアニメーションを再生
    /// </summary>
    /// <param name="type">アニメーションタイプ</param>
    /// <param name="isLoop">ループするか</param>
    void PlayAnimation(AnimationType type, bool isLoop = false);

    /// <summary>
    /// アニメーション名で直接再生
    /// </summary>
    /// <param name="animationName">アニメーション名</param>
    /// <param name="isLoop">ループするか</param>
    /// <returns>再生成功したらtrue</returns>
    bool PlayAnimationByName(const std::string& animationName, bool isLoop = false);

    /// <summary>
    /// ジャンプ処理
    /// </summary>
    /// <param name="speed">速度</param>
    /// <param name="fallSpeedLimit">落下速度制限</param>
    /// <param name="gravity">重力</param>
    void Jump(float& speed, float fallSpeedLimit, float gravity);

    /// <summary>
    /// 落下処理
    /// </summary>
    /// <param name="speed">速度</param>
    /// <param name="fallSpeedLimit">落下速度制限</param>
    /// <param name="gravity">重力</param>
    /// <param name="isJump">ジャンプフラグ</param>
    void Fall(float& speed, float fallSpeedLimit, float gravity, const bool& isJump);

    /// <summary>
    /// ターゲットへの方向取得
    /// </summary>
    /// <param name="target">ターゲット位置</param>
    Vector3 GetDirectionToTarget(const Vector3& target);

    /// <summary>
    /// ダメージ処理
    /// </summary>
    /// <param name="damageValue">ダメージ量</param>
    void TakeDamage(float damageValue);

    // ヒットクールタイム開始
    void StartDamageColling(float collingTime, const std::string& reactiveAttackName);

    // behavior変更
    void ChangeDamageReactionBehavior(std::unique_ptr<BaseEnemyDamageReaction> behavior);
    void ChangeBehavior(std::unique_ptr<BaseEnemyBehavior> behavior);

    void BackToDamageRoot(); //< ダメージルートに戻る

    /// ====================================================================
    /// Collision
    /// ====================================================================

    // 衝突コールバック
    void OnCollisionEnter([[maybe_unused]] BaseCollider* other) override;
    void OnCollisionStay([[maybe_unused]] BaseCollider* other) override;

    Vector3 GetCollisionPos() const override;

private:
    /// <summary>
    /// 視野内判定
    /// </summary>
    /// <param name="viewProjection">ビュープロジェクション</param>
    bool IsInView(const KetaEngine::ViewProjection& viewProjection) const;

    // ダメージクールタイムの更新
    void DamageCollingUpdate(float deltaTime);

    // ダメージリアクション変更処理
    void ChangeDamageReactionByPlayerAttack(PlayerAttackCollisionBox* attackController);

private:
    void MoveToLimit();

private:
    // 敵グループのID
    int32_t groupId_;

    // HPバーのサイズ
    Vector2 hpBarSize_;

    // other class
    Player* pPlayer_;
    Combo* pCombo_;
    GameCamera* pGameCamera_;
    EnemyManager* pEnemyManager_;
  

    // flags
    bool isDeathPending_    = false;
    bool isDamageColling_;
    bool isDeath_;
    bool isCollisionRope_;
    bool isInAnticipation_ = false; // 現在前隙中かどうか
    bool isAttacking_ = false;      // 攻撃中かどうか（押し戻し無効用）

    // damageInfo
    float damageCollTime_;
    std::string lastReceivedAttackName_;

protected:
    /// <summary>
    /// アニメーション名を設定
    /// </summary>
    void SetAnimationName(AnimationType type, const std::string& name) {
        animationNames_[static_cast<size_t>(type)] = name;
    }

    /// <summary>
    /// アニメーション名を取得
    /// </summary>
    const std::string& GetAnimationName(AnimationType type) const {
        return animationNames_[static_cast<size_t>(type)];
    }

protected:
    // 攻撃戦略
    std::unique_ptr<IEnemyAttackStrategy> attackStrategy_;

    std::unique_ptr<KetaEngine::Object3DAnimation> objAnimation_;

    std::unique_ptr<EnemyAttackCollisionBox> attackCollisionBox_;
    std::unique_ptr<EnemyHPBar> hpBar_;
    std::unique_ptr<EnemyEffects> enemyEffects_;

    // flags
    bool isReturningFromAttack_;

    // アニメーション関連
    std::array<std::string, static_cast<size_t>(AnimationType::Count)> animationNames_;
    ChaseAnimationState chaseAnimState_ = ChaseAnimationState::NONE;
    bool isPreDashFinished_ = false;

    /// behavior
    std::unique_ptr<BaseEnemyDamageReaction> damageBehavior_ = nullptr;
    std::unique_ptr<BaseEnemyBehavior> moveBehavior_         = nullptr;

    // パラメータ
    Parameter parameter_;

    // Type
    Type type_;

    // HP
    float hp_;
    float HPMax_;

public:
    /// ========================================================================================
    ///  getter method
    /// ========================================================================================
    const bool& GetIsDeath() const { return isDeath_; }
    const bool& GetIsDeathPending() const { return isDeathPending_; }
    const Type& GetType() const { return type_; }
    const Parameter& GetParameter() const { return parameter_; }
    int32_t GetGroupId() const { return groupId_; }
    float GetHP() const { return hp_; }
    Vector3 GetBodyRotation() const { return objAnimation_->transform_.rotation_;}
    Player* GetPlayer() const { return pPlayer_; }
    GameCamera* GetGameCamera() const { return pGameCamera_; }
    BaseEnemyDamageReaction* GetDamageReactionBehavior() const { return damageBehavior_.get(); }
    EnemyManager* GetManager() const { return pEnemyManager_; }
    EnemyEffects* GetEnemyEffects() const { return enemyEffects_.get(); }
    EnemyAttackCollisionBox* GetAttackCollisionBox() const { return attackCollisionBox_.get(); }
    KetaEngine::Object3DAnimation* GetAnimationObject() const { return objAnimation_.get(); }
    bool GetIsReturningFromAttack() const { return isReturningFromAttack_; }
    bool IsInAnticipation() const { return isInAnticipation_; }
    bool IsAttacking() const { return isAttacking_; }
    ChaseAnimationState GetChaseAnimState() const { return chaseAnimState_; }
    std::vector<std::string> GetAnimationNames() const;

    /// ========================================================================================
    ///  setter method
    /// ========================================================================================
    void SetPlayer(Player* plyaer);
    void SetGameCamera(GameCamera* gamecamera);
    void SetManager(EnemyManager* manager);
    void SetCombo(Combo* combo);
    void SetParameter(const Type& type, const Parameter& paramater);
    void SetBodyRotateX(float rotate) { objAnimation_->transform_.rotation_.x = rotate; }
    void SetBodyColor(const Vector4& color);
    void SetIsDeath(const bool& is) { isDeath_ = is; }
    void SetGroupId(const int& groupId) { groupId_ = groupId; }
    void SetIsDeathPending(const bool& is) { isDeathPending_ = is; }
    void SetWorldPositionY(float PosY) { baseTransform_.translation_.y = PosY; }
    void SetIsReturningFromAttack(bool is) { isReturningFromAttack_ = is; }
    void SetIsInAnticipation(bool value) { isInAnticipation_ = value; }
    void SetIsAttacking(bool value) { isAttacking_ = value; }
};
