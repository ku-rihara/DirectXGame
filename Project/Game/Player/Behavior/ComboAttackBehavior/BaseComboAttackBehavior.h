#pragma once

#include "BaseBehavior/BaseBehavior.h"
#include "Easing/Easing.h"
#include <memory>

class Player;
class PlayerParameter;

/// <summary>
/// コンボ攻撃振る舞いの基底クラス
/// </summary>
class BaseComboAttackBehavior : public BaseBehavior<Player> {
public:
    BaseComboAttackBehavior(const std::string& name, Player* player);
    virtual ~BaseComboAttackBehavior() = default;

    virtual void Init()                 = 0;
    virtual void Update(float atkSpeed) = 0;
    virtual void Debug() override       = 0;
    virtual void ChangeNextCombo(std::unique_ptr<BaseComboAttackBehavior> nextCombo);

protected:
    // プレイヤーパラメータへのポインタ
    PlayerParameter* pPlayerParameter_ = nullptr;
};