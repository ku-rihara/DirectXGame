#pragma once

#include"Rail.h"
//math
#include"Vector3.h"
//3d
#include"ViewProjection.h"
#include"Model.h"
//std
#include<list>
#include<memory>

class RailManager {
private:
	std::list<std::unique_ptr<Rail>>rails_;
	Model* model_;

public:
	void AddRail(const Vector3&pos);
	void Init();
	void Update();
	void Draw(const ViewProjection& viewProjection);
};