#pragma once

#include<string>
#include"Vector3.h"

class BaseEnemy;
class BaseEnemyDamageReaction{
protected:
	//振る舞い名
	std::string name_;
	//ボス
	BaseEnemy* pBaseEnemy_ = nullptr;

	Vector3 direction_;
	float objectiveAngle_;
public:

	BaseEnemyDamageReaction(const std::string& name, BaseEnemy* boss) : name_(name), pBaseEnemy_(boss) {}
	
	virtual ~BaseEnemyDamageReaction() {}

	virtual void Update()=0;
	virtual void Debug() = 0;

	
};