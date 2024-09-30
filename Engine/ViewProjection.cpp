#include "ViewProjection.h"
#include"DirectXCommon.h"
#include <DirectXMath.h>
#include<assert.h>
void ViewProjection::Init() {
	//定数バッファ生成
	CreateConstantBuffer();
	//マッピング
	Map();
	//ビュー行列の更新
	UpdateViewMatrix();
	//射影行列の更新
	UpdateProjectionMatrix();
	//行列の転送
	TransferMatrix();
}

void ViewProjection::CreateConstantBuffer() {
	//デバイスの取得
	Microsoft::WRL::ComPtr<ID3D12Device> device = DirectXCommon::GetInstance()->GetDevice();
	//定数バッファのサイズを計算
	const UINT bufferSize = sizeof(ConstBufferDataViewProjection);

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

void ViewProjection::Map() {
	//定数バッファのマッピング
	D3D12_RANGE readRange = {};
	HRESULT hr = constBuffer_->Map(0, &readRange, reinterpret_cast<void**>(&constMap));
	assert(SUCCEEDED(hr));
}

void ViewProjection::TransferMatrix() {
	//定数バッファに行列データを転送する
	constMap->view = matView_;
	constMap->projection = matProjection_;
	constMap->cameraPos = translation_;
}


void ViewProjection::UpdateMatrix() {
	//ビュー行列の更新
	UpdateViewMatrix();
	//射影行列の更新
	UpdateProjectionMatrix();
	//行列の転送
	TransferMatrix();
}

void ViewProjection::UpdateViewMatrix() {
	//回転行列を計算
	Matrix4x4 rotateMatrix = MakeRotateMatrix(rotation_);
	//平行移動行列を計算
	Matrix4x4 translateMatrx = MakeTranslateMatrix(translation_);
	//ビュー行列を計算
	Matrix4x4 cameraMatrix = rotateMatrix * translateMatrx;
	matView_ = Inverse(cameraMatrix);
}

void ViewProjection::UpdateProjectionMatrix() {
	matProjection_ = MakePerspectiveFovMatrix(fovAngleY_, aspectRatio_, nearZ_, farZ_);

}