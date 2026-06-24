#pragma once

#include "BaseBehavior/BaseBehavior.h"
#include "Editor/Easing/Easing.h"
#include <memory>

class Player;
class PlayerParameter;

/// <summary>
/// コンボ攻撃振る舞いの基底クラス
/// </summary>
class BaseComboAttackBehavior : public BaseBehavior<Player> {
public:
    enum class Type { Base, Root, Action };

    BaseComboAttackBehavior(const std::string& name, Player* player);
    virtual ~BaseComboAttackBehavior() = default;

    virtual void Init()                 = 0;
    virtual void Update(float atkSpeed) = 0;
    virtual void Debug() override       = 0;
    virtual void ChangeNextCombo(std::unique_ptr<BaseComboAttackBehavior> nextCombo);
    virtual Type GetComboType() const { return Type::Base; }

protected:
    // プレイヤーパラメータへのポインタ
    PlayerParameter* pPlayerParameter_ = nullptr;
};