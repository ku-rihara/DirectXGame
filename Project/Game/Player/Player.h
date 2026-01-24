#pragma once

#include"3d/WorldTransform.h"
#include"3d/ViewProjection.h"
#include"3d/Object3d/Object3d.h"
#include<memory>

class Player {
private:
	WorldTransform transform_;
	std::unique_ptr<Object3d>obj3d_;
	float speed_;
public:
	void Init();
	void Update();
	void Draw(const ViewProjection&viewPro);

	void MoveRight();
	void MoveLeft();
};