#pragma once
#include<wrl.h>
#include <d3d12.h>
#include"Vector3.h"
#include"Matrix4x4.h"

// 定数バッファ用データ構造体
struct ConstBufferDataViewProjection {
	Matrix4x4 view;       // ビュー変換行列
	Matrix4x4 projection; // プロジェクション変換行列
	Vector3 cameraPos;    // カメラ座標
};

class ViewProjection{
public:
	//scale
	Vector3 scale_{ 1,1,1 };
	//rotate
	Vector3 rotation_ = { 0, 0, 0 };
	// translate
	Vector3 translation_ = { 0, 0, 0 };

	// 垂直方向視野角
	float fovAngleY_ = 45.0f * 3.141592654f / 180.0f;
	// ビューポートのアスペクト比
	float aspectRatio_ = (float)16 / 9;
	// 深度限界
	float nearZ_ = 0.1f;
	// 深度限界
	float farZ_ = 1000.0f;
	// ビュー行列
	Matrix4x4 matView_;
	// 射影行列
	Matrix4x4 matProjection_;
	//カメラ行列
	Matrix4x4 cameraMatrix_;

private:
	// 定数バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuffer_;
	// マッピング済みアドレス
	ConstBufferDataViewProjection* constMap = nullptr;
	// コピー禁止
	ViewProjection(const ViewProjection&) = delete;
	ViewProjection& operator=(const ViewProjection&) = delete;
	
public:
	ViewProjection() = default;
	~ViewProjection() = default;
	/// <summary>
	/// 初期化
	/// </summary>
	void Init();
	/// <summary>
		/// 定数バッファ生成
		/// </summary>
	void CreateConstantBuffer();

	/// <summary>
	/// マッピング
	/// </summary>
	void Map();

	/// <summary>
	/// 行列転送
	/// </summary>
	void TransferMatrix();

	/// <summary>
	/// 行列の更新
	/// </summary>
	void UpdateMatrix();

	/// <summary>
	/// ビュー行列の更新
	/// </summary>
	void UpdateViewMatrix();

	/// <summary>
	/// 射影行列を更新する
	/// </summary>
	void UpdateProjectionMatrix();

	/// <summary>
	/// 定数バッファの取得
	/// </summary>
	/// <returns>定数バッファ</returns>
	const Microsoft::WRL::ComPtr<ID3D12Resource>& GetConstBuffer() const { return constBuffer_; }

	/// <summary>
	/// カメラ行列取得
	/// </summary>
	/// <returns></returns>
	const Matrix4x4& GetCameraMatrix()const { return cameraMatrix_; }

};

