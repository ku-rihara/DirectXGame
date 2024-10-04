#include "WorldTransform.h"
#include"DirectXCommon.h"
#include<assert.h>
#include<numbers>


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
		matWorld_ =  parent_->matWorld_* matWorld_;
	}
	// 定数バッファに転送する
	TransferMatrix();
}


void WorldTransform::BillboardUpdateMatrix(const ViewProjection& viewProjection) {
	Matrix4x4 scaleMatrix = MakeScaleMatrix(scale_);
	// 回転行列を計算
	Matrix4x4 rotateMatrix = MakeRotateMatrix(rotation_);
	// 平行移動行列を計算
	Matrix4x4 translateMatrix = MakeTranslateMatrix(translation_);
	backToFrontMatrix_ = MakeRotateYMatrix(std::numbers::pi_v<float>);
	
		billboardMatrix_ = backToFrontMatrix_* viewProjection.GetCameraMatrix();
		billboardMatrix_.m[3][0] = 0.0f;
		billboardMatrix_.m[3][1] = 0.0f;
		billboardMatrix_.m[3][2] = 0.0f;
	
		matWorld_ = scaleMatrix * billboardMatrix_ * translateMatrix;

		// 親子関係があれば親のワールド行列を掛ける
		if (parent_) {
			matWorld_ = parent_->matWorld_* matWorld_;
		}
		// 定数バッファに転送する
		TransferMatrix();
}
