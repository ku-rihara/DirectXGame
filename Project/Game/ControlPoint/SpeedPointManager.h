#pragma once

#include"ControlPoint/BaseSpeedControl.h"

//math
#include"Vector3.h"
//3d
#include"3d/ViewProjection.h"
#include"3d/Model.h"
//std
#include<list>
#include<memory>
// nlohmann/jsonライブラリを使うためのインクルード
#include <json.hpp>
using json = nlohmann::json;

class SpeedPointManager {
private:
	std::list<std::unique_ptr<BaseSpeedControl>>rails_;

	std::vector<Vector3>slowSpeed_;
	std::vector<Vector3>fastSpeed_;
	Model* model_;
	int controlNum_;
public:
	void AddSlowSpeed(const Vector3&pos);
	void AddFastSpeed(const Vector3& pos);

	void Init();
	void Update();
	void Draw(const ViewProjection& viewProjection);
	void Debug();
	// 制御点をファイルに保存
	void SaveControlSpots(const std::string& filename);

	// 制御点をファイルから読み込み
	void LoadSlowSpots(const std::string& filename);

	std::vector<Vector3> GetControlPoints()const { return slowSpeed_; }
	
};