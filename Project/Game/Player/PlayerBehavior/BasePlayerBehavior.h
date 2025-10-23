#pragma once

#include <string>

class Player;
class PlayerParameter;

/// <summary>
/// プレイヤー振る舞いの基底クラス
/// </summary>
class BasePlayerBehavior {
protected:
    // 振る舞い名
    std::string name_;
    // ボス
    Player* pPlayer_                   = nullptr;
    PlayerParameter* pPlayerParameter_ = nullptr;

public:
    BasePlayerBehavior(const std::string& name, Player* player);

    virtual ~BasePlayerBehavior() {}

    virtual void Update() = 0; //< 更新
    virtual void Debug()  = 0; //< デバッグ
};