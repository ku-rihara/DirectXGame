#pragma once
#include <memory>
#include "Player/Behavior/PlayerBehavior/BasePlayerBehavior.h"
#include "Player/Behavior/ComboAttackBehavior/BaseComboAttackBehavior.h"

class Player;

/// <summary>
/// プレイヤーの行動を管理するクラス
/// </summary>
class PlayerBehaviorManager {
public:
    void Init(Player* player);

    // 行動切り替え
    void Change(std::unique_ptr<BasePlayerBehavior> behavior);
    void ChangeCombo(std::unique_ptr<BaseComboAttackBehavior> behavior);
    void ResetComboToRoot(Player* player);
    void EnterDeath(Player* player);
    void StartAutoDash(Player* player);
    void ClearForceDash();

    // 現在の行動取得
    BasePlayerBehavior*      GetBehavior()      const { return behavior_.get(); }
    BaseComboAttackBehavior* GetComboBehavior() const { return comboBehavior_.get(); }

    /// 指定型にキャストして取得
    template<typename T> T* GetAs() const;
    template<typename T> T* GetComboAs() const;

    // 状態クエリ
    bool IsDashing()      const;
    bool IsStartDashing() const; 
    bool IsAirborne()     const;
    bool IsComboRoot() const;
    bool IsDead()      const;
    bool IsSpawning()  const;

private:
    std::unique_ptr<BasePlayerBehavior>      behavior_;      ///< 通常行動
    std::unique_ptr<BaseComboAttackBehavior> comboBehavior_; ///< コンボ攻撃行動
};
