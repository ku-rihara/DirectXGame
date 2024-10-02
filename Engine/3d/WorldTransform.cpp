#include "WorldTransform.h"
#include"DirectXCommon.h"
#include<assert.h>


WorldTransform::WorldTransform() {

}

WorldTransform::~WorldTransform() {
	
}


void WorldTransform::Init() {
	//ローカルスケール
	scale_ = { 1,1,1 };
	//ローカル回転角
	rotation_ = {};
	//ローカル座標
	translation_ = {};

	//定数バッファの生成
	CreateConstantBuffer();
	//マッピング
	Map();
	// 行列の更新
	UpdateMatrix();
}

void WorldTransform::CreateConstantBuffer() {
	//デバイスの取得
	Microsoft::WRL::ComPtr<ID3D12Device> device = DirectXCommon::GetInstance()->GetDevice();
	//定数バッファのサイズを計算
	const UINT bufferSize = sizeof(ConstBufferDataWorldTransform);

	//定数バッファを生成
	constBuffer_ = DirectXCommon::GetInstance()->CreateBufferResource(device, bufferSize);
}

void WorldTransform::Map() {
	//定数バッファのマッピング
	D3D12_RANGE readRange = {};
	HRESULT hr = constBuffer_->Map(0, &readRange, reinterpret_cast<void**>(&constMap));
	assert(SUCCEEDED(hr));
}

void WorldTransform::TransferMatrix() {
	//定数バッファに行列データを転送する
	if (constMap) {
		constMap->matWorld = matWorld_;
	}
}


void WorldTransform::UpdateMatrix() {
	// スケール、回転、平行移動を合成して行列を計算する
	matWorld_ = MakeAffineMatrix(scale_, rotation_, translation_);
	// 親子関係があれば親のワールド行列を掛ける
	if (parent_) {
		matWorld_ = matWorld_* parent_->matWorld_;
	}
	// 定数バッファに転送する
	TransferMatrix();
}
