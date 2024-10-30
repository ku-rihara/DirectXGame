#pragma once

#include"ControlPoint.h"

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

class ControlPointManager {
private:
	std::list<std::unique_ptr<ControlPoint>>rails_;
	std::vector<Vector3>controlSpots_;
	Model* model_;
	int controlNum_;
public:
	void AddControlPoint(const Vector3&pos);
	void Init();
	void Update();
	void Draw(const ViewProjection& viewProjection);
	// 制御点をファイルに保存
	void SaveControlSpots(const std::string& filename);

	// 制御点をファイルから読み込み
	void LoadControlSpots(const std::string& filename);

	std::vector<Vector3> GetControlPoints()const { return controlSpots_; }
	
};