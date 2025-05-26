#include "ViewProjection.h"
#include"base/DirectXCommon.h"
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
}

void ViewProjection::Map() {
	//定数バッファのマッピング
    D3D12_RANGE readRange = {};
    HRESULT hr            = constBuffer_->Map(0, &readRange, reinterpret_cast<void**>(&constMap));
    assert(SUCCEEDED(hr));

    if (FAILED(hr)) {
        OutputDebugStringA("Map failed.\n");
     
    }
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
	////拡大縮小行れ宇
	//Matrix4x4 scaleMatrix = MakeScaleMatrix(scale_);
	// 回転行列を計算
	Matrix4x4 rotateMatrix = MakeRotateMatrix(rotation_);
	// 平行移動行列を計算
	Matrix4x4 translateMatrix = MakeTranslateMatrix(translation_);

	// カメラ行列を作成
	 cameraMatrix_ = rotateMatrix* translateMatrix;
	// 最終的なビュー行列の設定
	matView_ = Inverse(cameraMatrix_);
}

void ViewProjection::UpdateProjectionMatrix() {
	matProjection_ = MakePerspectiveFovMatrix(fovAngleY_, aspectRatio_, nearZ_, farZ_);

}

Vector3 ViewProjection::GetWorldPos() const {
	// カメラ行列(cameraMatrix_)の位置成分を取得する
	return Vector3(
		cameraMatrix_.m[3][0], // X成分
		cameraMatrix_.m[3][1], // Y成分
		cameraMatrix_.m[3][2]  // Z成分
	);
}
