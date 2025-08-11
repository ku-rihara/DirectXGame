#pragma once

#include "BaseAABBCollisionBox.h"
#include "Combo/Combo.h"
#include "utility/ParameterEditor/GlobalParameter.h"
#include <array>
#include <string>

class Combo;
class PlayerAttackController : public BaseAABBCollisionBox {
public:
    enum class AttackType {
        NORMAL,
        FALL,
        RUSH,
        STOPPER,
        THRUST,
        UPPER,
        COUNT,
    };

    enum class AttackValueMode {
        AttackSpeed,
        AttackPower,
    };

public:
    struct AttackValueForLevel {
        float speed;
        float power;
    };

    struct ColliderParameter {
        Vector3 size;
        float offsetValue;
        float adaptTime;
    };

public:
    PlayerAttackController()           = default;
    ~PlayerAttackController() override = default;

    void Init() override;
    void Update() override;
    void Draw() override;

    void TimerUpdate(const float& deltaTime);

    void SetPlayerBaseTransform(const WorldTransform* playerBaseTransform);
    void SetParentTransform(WorldTransform* transform) override;

    void ChangeAttackType(AttackType attackType);
    float GetAttackValue(AttackValueMode attackValueMode);

    void OnCollisionEnter([[maybe_unused]] BaseCollider* other) override;
    void OnCollisionStay([[maybe_unused]] BaseCollider* other) override;

    ///-------------------------------------------------------------------------------------
    /// Editor
    ///-------------------------------------------------------------------------------------
    void AdjustParam();
    void BindParams();

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
    std::array<ColliderParameter, static_cast<size_t>(AttackType::COUNT)> collisionParam_;

public:
    AttackType attackType_;

public: // accessor
    Vector3 GetCollisionPos() const override;
    bool GetIsSlow() const { return isSlow_; }
    bool GetIsHitStop() const { return isHitStop_; }
    AttackType GetTagList() const { return attackType_; }

    void SetIsSlow(const bool& is) { isSlow_ = is; }
    void SetIsHitStop(const bool& is) { isHitStop_ = is; }
    void SetCombo(Combo* combo);
};