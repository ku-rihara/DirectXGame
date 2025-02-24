#pragma once

#include<string>
#include"Vector3.h"

class BaseEnemy;
class BaseEnemyBehavior{
protected:
	//振る舞い名
	std::string name_;
	//ボス
	BaseEnemy* pBaseEnemy_ = nullptr;

	Vector3 direction_;
	float objectiveAngle_;
public:

	BaseEnemyBehavior(const std::string& name, BaseEnemy* boss) :name_(name), pBaseEnemy_(boss) {}
	
	virtual ~BaseEnemyBehavior() {}

	virtual void Update()=0;

	virtual void Debug() = 0;

	void AngleCaluclation();
};