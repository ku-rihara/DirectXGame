#pragma once

#include<string>

#include<memory>

class Player;
class BaseComboAattackBehavior {
protected:

	std::string name_;          //振る舞い名
	Player* pPlayer_ = nullptr; /// プレイヤー

	bool isNextCombo_;

public:

	BaseComboAattackBehavior(const std::string& name, Player* player) :name_(name), pPlayer_(player) {}
	virtual ~BaseComboAattackBehavior() {}
	
	virtual void Update()=0;   /// 更新
	virtual void Debug() = 0;  /// デバッグ

	virtual void ChangeNextComboFragForButton();
	virtual void ChangeNextCombo(std::unique_ptr<BaseComboAattackBehavior>nextCombo);
};