#pragma once

//math
#include"Vector3.h"
//3d
#include"3d/ViewProjection.h"
#include"3d/Model.h"
//std
#include<list>
#include<memory>
//obj
#include"BaseEnemy.h"
#include"Enemy/NormalEnemy.h"
// nlohmann/jsonライブラリを使うためのインクルード
#include <json.hpp>
using json = nlohmann::json;

class EnemyManager {
private:
	std::list<std::unique_ptr<BaseEnemy>>enemies_;/// 敵リスト
	std::vector<Vector3>positions_;/// 座標
	Model* model_;
	bool isAdaption_;
	int controlNum_;
public:
	void AddNormalEnemy(const Vector3&pos);
	void Init();
	void Update();
	void Draw(const ViewProjection& viewProjection);
	// 制御点をファイルに保存
	void SaveEnemyPoses(const std::string& filename);

	// 制御点をファイルから読み込み
	void LoadEnemyPosies(const std::string& filename);

	std::vector<Vector3> GetPositions()const { return positions_; }
	
};