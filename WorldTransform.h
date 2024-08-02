#pragma once
#include"Vector3.h"
#include"Matrix4x4.h"
#include <wrl.h>
#include <d3d12.h>
// 定数バッファ用データ構造体
struct ConstBufferDataWorldTransform {
	Matrix4x4 matWorld; // ローカル → ワールド変換行列
};
/// <summary>
/// ワールド変換データ
/// </summary>
class WorldTransform{
public:
	//ローカルスケール
	Vector3 scale_ = { 1,1,1 };
	//ローカル回転角
	Vector3 rotation_ = {};
	//ローカル座標
	Vector3 translation_ = {};
	//ローカル→ワールド変換行列
	Matrix4x4 matWorld_;
	//親となるワールド変換へのポインタ
	const WorldTransform* parent_ = nullptr;
private:
	// 定数バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuffer_;
	// マッピング済みアドレス
	ConstBufferDataWorldTransform* constMap = nullptr;
	// コピー禁止
	WorldTransform(const WorldTransform&) = delete;
	WorldTransform& operator=(const WorldTransform&) = delete;
public:
	WorldTransform() = default;
	~WorldTransform() = default;

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
	/// 定数バッファの取得
	/// </summary>
	/// <returns>定数バッファ</returns>
	const Microsoft::WRL::ComPtr<ID3D12Resource>& GetConstBuffer() const { return constBuffer_; }


};

