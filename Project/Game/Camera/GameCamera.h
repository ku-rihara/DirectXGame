#pragma once
//std
#include<vector>
#include<memory>
//math
#include<Vector3.h>
//3D
#include"3d/ViewProjection.h"
#include"3d/WorldTransform.h"
#include"3d/Object3d.h"
class GameCamera {
private:
	WorldTransform worldTransform_;
	ViewProjection viewProjection_;
	// 線分の数
	const size_t IndexCount = 500;
	float railMoveTime_;
	Vector3 cameraRotate_;
	int RailIndex = 0;
	std::vector<Vector3> pointsDrawing;
	
	//モデル(本来はレールクラス)
	std::vector <WorldTransform>transform_;
	std::vector<std::unique_ptr<Object3d>>object3d_;

	float railMoveSpeed_ = 0.1f;  // 移動速度の調整

public:
	//Function
	void Init();
	void Update(const std::vector<Vector3>&controlPos);
	void RailDraw(const ViewProjection& viewProjection);
	void Debug();
	
	/// <summary>
	/// プロジェクション行列計算用のメンバ設定関数群
	/// </summary>
	void SetFovAngleY(float value) { viewProjection_.fovAngleY_ = value; }
	void SetAspectRatio(float value) { viewProjection_.aspectRatio_ = value; }
	void SetNearZ(float value) { viewProjection_.nearZ_ = value; }
	void SetFarZ(float value) { viewProjection_.farZ_ = value; }

	//getter
	const WorldTransform& GetWorldTransform() const { return worldTransform_; }
	const ViewProjection& GetViewProjection() const { return viewProjection_; }
};