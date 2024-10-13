#pragma once

#include"Rail.h"
#include "GlobalParameter.h"
//math
#include"Vector3.h"
//3d
#include"ViewProjection.h"
#include"Model.h"
//std
#include<list>
#include<memory>
class GameCamera;
class RailManager {
private:
	/*GameCamera* gameCamera_;*/
	std::list<std::unique_ptr<Rail>>rails_;
	std::vector<Vector3>controlSpot_;
	Model* model_;
	bool isAdaption_;
	GlobalParameter* globalParameter_;
	int controlNum_;
public:
	void AddRail(const Vector3&pos);
	void Init();
	void Update();
	void Draw(const ViewProjection& viewProjection);

	void ApplyGlobalParameter();

	//制御点適応
	void AdaptationControlSpot();
};