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

	//HRESULT hr;
	////頂点リソース用のヒープの設定
	//D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	//uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;//UploadHeapを使う
	////頂点リソース用設定
	//D3D12_RESOURCE_DESC vertexResourceDesc{};
	////ばっぱリソース。テクスチャの場合はまた別の設定をする
	//vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	//vertexResourceDesc.Width = bufferSize;//リソースのサイズ。今回はVector4を3頂点文
	////バッファの場合はこれらは1にする決まり
	//vertexResourceDesc.Height = 1;
	//vertexResourceDesc.DepthOrArraySize = 1;
	//vertexResourceDesc.MipLevels = 1;
	//vertexResourceDesc.SampleDesc.Count = 1;
	////バッファの場合これにする決まり
	//vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//
	//hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
	//	&vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&constBuffer_));
	//assert(SUCCEEDED(hr));

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
