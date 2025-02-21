#pragma once
#include"Vector3.h"
#include"Matrix4x4.h"
#include"3d/ViewProjection.h"
#include"input/Input.h"
class DebugCamera{
	// カメラ注視点までの距離
	static const float distance_;
private://メンバ変数
	//入力クラスのポインタ
	Input* input_;
	//スケーリング
	float scaleX_ = 1.0f;
	float scaleY_ = 1.0f;

	float yaw_;
	float pitch_;
	//ビュープロジェクション
	ViewProjection viewProjection_;
	//回転行列
	Matrix4x4 matRot_;
	//アクティブ化
	bool isActive_;
private:
	/// <summary>
	/// 行列更新
	/// </summary>
	void UpdateMatrix();
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="window_width">画面幅</param>
	/// <param name="window_height">画面高さ</param>
	DebugCamera(int window_width, int window_height);

	/// <summary>
	/// 初期化
	/// </summary>
	void Init();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();


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


	void SetIsActive(bool is) { isActive_ = is; }
};

