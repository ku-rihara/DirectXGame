#pragma once

#include "BaseAABBCollisionBox.h"
#include "Combo/Combo.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include <array>
#include <string>

class PlayerComboAttackData;
/// <summary>
/// プレイヤー攻撃の制御クラス
/// </summary>
class PlayerCollisionInfo : public BaseAABBCollisionBox {
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
    void TimerUpdate(const float& deltaTime);
    void UpdateOffset();

    // 攻撃情報取得
    void GetAttackInfo(const PlayerComboAttackData* comboAttackData);

    //コリジョンコールバック
    void OnCollisionStay([[maybe_unused]] BaseCollider* other) override;

private:
    const WorldTransform* baseTransform_          = nullptr;
    const PlayerComboAttackData* comboAttackData_ = nullptr;

    // 状態
    float adaptTimer_;
    bool isCollision_;

public:
    Vector3 GetCollisionPos() const override;
    const PlayerComboAttackData* GetComboAttackData() const { return comboAttackData_; }

    void SetIsCollision(const bool& is) { isCollision_ = is; }
    void SetPlayerBaseTransform(const WorldTransform* playerBaseTransform);
    void SetParentTransform(WorldTransform* transform) override;
};