#pragma once

#include<string>

class GameCamera;
class BaseGameCameraBehavior {
protected:
	//振る舞い名
	std::string name_;
	//ゲームカメラ
	GameCamera* pGameCamera_ = nullptr;
public:

	BaseGameCameraBehavior(const std::string& name, GameCamera* camera) :name_(name), pGameCamera_(camera) {}
	
	virtual ~BaseGameCameraBehavior() {}

	virtual void Update()=0;

	virtual void Debug() = 0;
};