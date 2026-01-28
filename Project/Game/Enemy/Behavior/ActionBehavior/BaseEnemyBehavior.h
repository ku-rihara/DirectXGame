#pragma once

#include<string>

class BaseEnemy;
class BaseEnemyBehavior{
protected:
	//振る舞い名
	std::string name_;
	//ボス
	BaseEnemy* pBaseEnemy_ = nullptr;
public:

	BaseEnemyBehavior(const std::string& name, BaseEnemy* boss) :name_(name), pBaseEnemy_(boss) {}
	
	virtual ~BaseEnemyBehavior() {}

	virtual void Update()=0;

	virtual void Debug() = 0;

};