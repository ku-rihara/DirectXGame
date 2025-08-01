#pragma once

#include<string>
#include"Vector3.h"


class GameCamera;
class BaseGameCameraBehavior {
protected:
	//振る舞い名
	std::string name_;
	//ゲームカメラ
	GameCamera* pGameCamera_ = nullptr;

	Vector3 saveOffset_;
public:

	BaseGameCameraBehavior(const std::string& name, GameCamera* camera) :name_(name), pGameCamera_(camera) {}
	
	virtual ~BaseGameCameraBehavior() {}

	virtual void Update()=0;

	virtual void Debug() = 0;
};