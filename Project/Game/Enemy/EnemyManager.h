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
// nlohmann/jsonライブラリを使うためのインクルード
#include <json.hpp>
using json = nlohmann::json;

///============================================
///前方宣言
/// ===========================================
class Score;
class EnemyManager {
private:
	///============================================
	///private variants
	/// ===========================================
	std::list<std::unique_ptr<BaseEnemy>>enemies_;/// 敵リスト
	std::vector<Vector3>positions_;/// 座標
	Model* model_;
	bool isAdaption_;
	int controlNum_;

	/// other class
	Score* pScore_;

public:

	/// ===================================================
	/// public  method
	/// ===================================================

	void AddNormalEnemy(const Vector3&pos);
	void Init();
	void Update(const Vector3& traget);
	void Debug();
	void Draw(const ViewProjection& viewProjection);

	/// other classSet
	void SetScore(Score* score);

	// 制御点をファイルに保存
	void SaveEnemyPoses(const std::string& filename);

	// 制御点をファイルから読み込み
	void LoadEnemyPosies(const std::string& filename);

	std::vector<Vector3> GetPositions()const { return positions_; }
	
};