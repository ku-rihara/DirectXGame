#pragma once
//std
#include<vector>
//math
#include<Vector3.h>
//3D
#include"ViewProjection.h"
class GameCamera {
private:
	ViewProjection viewProjection_;
public:
	//Function
	void Init();
	void Update(const std::vector<Vector3>&controlPos);
	void ControlSpotDraw();
	
	
	/// <summary>
	/// プロジェクション行列計算用のメンバ設定関数群
	/// </summary>
	void SetFovAngleY(float value) { viewProjection_.fovAngleY_ = value; }
	void SetAspectRatio(float value) { viewProjection_.aspectRatio_ = value; }
	void SetNearZ(float value) { viewProjection_.nearZ_ = value; }
	void SetFarZ(float value) { viewProjection_.farZ_ = value; }

	/// <summary>
	/// ビュープロジェクションを取得
	/// </summary>
	/// <returns>ビュープロジェクション</returns>
	const ViewProjection& GetViewProjection() { return viewProjection_; }
};