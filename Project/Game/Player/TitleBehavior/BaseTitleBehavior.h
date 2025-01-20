#pragma once

#include<string>
#include<memory>
#include"Easing.h"

class Player;
class BaseTitleBehavior {
protected:

	std::string name_;          //振る舞い名
	Player* pPlayer_ = nullptr; /// プレイヤー

	bool isNextCombo_;

	Easing startEasing_;
	

public:

	BaseTitleBehavior(const std::string& name, Player* player) :name_(name), pPlayer_(player) {}
	virtual ~BaseTitleBehavior() {}
	
	virtual void Update()=0;   /// 更新
	virtual void Debug() = 0;  /// デバッグ
};