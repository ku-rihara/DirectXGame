#pragma once

#include "BaseAABBCollisionBox.h"
#include "Combo/Combo.h"
#include "utility/ParameterEditor/GlobalParameter.h"
#include <array>
#include <string>

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

public:
    struct AttackValueForLevel {
        float speed[kComboLevel];
        float power[kComboLevel];
    };

public:
    PlayerAttackController()           = default;
    ~PlayerAttackController() override = default;

    void Init() override;
    void Update() override;
    void Draw() override;

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

    // 状態
    bool isSlow_;
    bool isHitStop_;

    AttackValueForLevel attackValueForLevel_;

public:
    AttackType attackType_;

public: // accessor
    Vector3 GetCollisionPos() const override;
    bool GetIsSlow() const { return isSlow_; }
    bool GetIsHitStop() const { return isHitStop_; }
    AttackType GetTagList() const { return attackType_; }

    void IsAdapt(bool is) override;
    void SetSize(const Vector3& size) override;
    void SetPosition(const Vector3& position) override;
    void SetOffset(const Vector3& offset) override;
    void SetIsSlow(const bool& is) { isSlow_ = is; }
    void SetIsHitStop(const bool& is) { isHitStop_ = is; }
};