#pragma once

#include "BaseAABBCollisionBox.h"
#include "Combo/Combo.h"
#include "utility/ParameterEditor/GlobalParameter.h"
#include <array>
#include <string>

class Combo;

/// <summary>
/// プレイヤー攻撃の制御クラス
/// </summary>
class PlayerCollisionInfo : public BaseAABBCollisionBox {
public:
    enum class AttackType {
        NORMAL,
        UPPER,
        THRUST,
        FALL,
        RUSH,
        COUNT,
    };

public:
    struct AttackValueForLevel {
        float speedRate;
        float powerRate;
    };

    struct AttackParameter {
        Vector3 collisionSize;
        float collisionOffsetValue;
        float adaptTime;
        float power;
    };

public:
    PlayerCollisionInfo()           = default;
    ~PlayerCollisionInfo() override = default;

    // 初期化、更新、描画
    void Init() override;
    void Update() override;
    void Draw() override;

    /// <summary>
    /// タイマー更新
    /// </summary>
    /// <param name="deltaTime">デルタタイム</param>
    void TimerUpdate(const float& deltaTime);

    /// <summary>
    /// プレイヤーのベーストランスフォーム設定
    /// </summary>
    /// <param name="playerBaseTransform">プレイヤーのベーストランスフォーム</param>
    void SetPlayerBaseTransform(const WorldTransform* playerBaseTransform);

    /// <summary>
    /// 親トランスフォームの設定
    /// </summary>
    /// <param name="transform">親のワールドトランスフォーム</param>
    void SetParentTransform(WorldTransform* transform) override;

    /// <summary>
    /// 攻撃タイプの変更
    /// </summary>
    /// <param name="attackType">攻撃タイプ</param>
    void ChangeAttackType(const AttackType& attackType);

    /// <summary>
    /// 攻撃速度取得
    /// </summary>
    /// <param name="baseTime">基本時間</param>
    float GetAttackSpeed(const float& baseTime);

    /// <summary>
    /// 攻撃タイプを文字列に変換
    /// </summary>
    /// <param name="type">攻撃タイプ</param>
    std::string GetAttackTypeName(const AttackType& type);

    /// <summary>
    /// 衝突開始時コールバック
    /// </summary>
    /// <param name="other">衝突相手のコライダー</param>
    void OnCollisionEnter([[maybe_unused]] BaseCollider* other) override;

    /// <summary>
    /// 衝突中コールバック
    /// </summary>
    /// <param name="other">衝突相手のコライダー</param>
    void OnCollisionStay([[maybe_unused]] BaseCollider* other) override;

    void AdjustParam(); //< パラメータ調整
    void BindParams(); //< パラメータバインド

    float GetAttackPower(); //< 攻撃力取得
    void ResetComboEffect(); //< コンボエフェクトリセット

private:
    GlobalParameter* globalParameter_;
    const std::string groupName_ = "PlayerAttack";

    const WorldTransform* baseTransform_ = nullptr;
    Combo* pCombo_;

    // 状態
    bool isSlow_;
    bool isHitStop_;
    float adaptTimer_;

    // レベルごとの攻撃パラメータ
    std::array<AttackValueForLevel, kComboLevel> attackValueForLevel_;

    // 各攻撃のColliderパラメータ
    std::array<AttackParameter, static_cast<size_t>(AttackType::COUNT)> attackParam_;

public:
    AttackType attackType_;
public:

    Vector3 GetCollisionPos() const override;
    const bool& GetIsSlow() const { return isSlow_; }
    const bool& GetIsHitStop() const { return isHitStop_; }
    const AttackType& GetTagList() const { return attackType_; }

    void SetIsSlow(const bool& is) { isSlow_ = is; }
    void SetIsHitStop(const bool& is) { isHitStop_ = is; }
    void SetCombo(Combo* combo);
};