#pragma once
/// obj
#include"3d/WorldTransform.h"
#include"3d/ViewProjection.h"
#include"3d/Object3d.h"
//obj
#include"ControlPoint/ControlPointManager.h"
#include"Enemy/EnemyManager.h"
/// std
#include<memory>

class PositionEditor {
private:

	/// ===================================================
	///protected variaus
	/// ===================================================

	std::unique_ptr<Object3d>obj3d_;// 配置プレビュー
	WorldTransform transform_;
	//変数
	const float mouseDepth_ = 0.995f;
public:
	/// ===================================================
	///public method
	/// ===================================================
	
	 /// 初期化、更新、描画
 	void Init();
	void Update(const ViewProjection& viewProjection);
	void PutControlPoint(ControlPointManager* controlPointManager);
	void PutEnemy(EnemyManager* enemyManager);
	void Draw(const ViewProjection& viewProjection);

	void Debug();//デバッグ

	/// ===================================================
   ///getter 
   /// ===================================================
};