#pragma once

#include<string>
#include"EasingFunction.h"

class Player;
class PlayerParameter;
class BaseTitleBehavior {
protected:

	std::string name_;          //振る舞い名
	Player* pPlayer_ = nullptr; /// プレイヤー
    PlayerParameter* pPlayerParameter_ = nullptr;

	bool isNextCombo_;
	Easing startEasing_;

public:
   
    BaseTitleBehavior(const std::string& name, Player* player);

	virtual ~BaseTitleBehavior() {}
	
	virtual void Update()=0;   /// 更新
	virtual void Debug() = 0;  /// デバッグ
};