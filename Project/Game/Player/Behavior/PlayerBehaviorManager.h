#pragma once
#include <memory>
#include "Player/Behavior/PlayerBehavior/BasePlayerBehavior.h"
#include "Player/Behavior/ComboAttackBehavior/BaseComboAttackBehavior.h"

class Player;

class PlayerBehaviorManager {
public:
    void Init(Player* player);

    void Change(std::unique_ptr<BasePlayerBehavior> behavior);
    void ChangeCombo(std::unique_ptr<BaseComboAttackBehavior> behavior);
    void ResetComboToRoot(Player* player);
    void EnterDeath(Player* player);
    void StartAutoDash(Player* player);
    void ClearForceDash();

    BasePlayerBehavior*      GetBehavior()      const { return behavior_.get(); }
    BaseComboAttackBehavior* GetComboBehavior() const { return comboBehavior_.get(); }

    template<typename T>
    T* GetAs() const { return dynamic_cast<T*>(behavior_.get()); }

    template<typename T>
    T* GetComboAs() const { return dynamic_cast<T*>(comboBehavior_.get()); }

    bool IsDashing()   const;
    bool IsAirborne()  const;
    bool IsComboRoot() const;
    bool IsDead()      const;
    bool IsSpawning()  const;

private:
    std::unique_ptr<BasePlayerBehavior>      behavior_;
    std::unique_ptr<BaseComboAttackBehavior> comboBehavior_;
};
