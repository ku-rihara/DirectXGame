#include "WorldTransform.h"
#include"base/DirectXCommon.h"
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
	D3D12_RANGE readRange = {};
	HRESULT hr = constBuffer_->Map(0, &readRange, reinterpret_cast<void**>(&constMap));
	if (FAILED(hr)) {
		throw std::runtime_error("Failed to map constant buffer.");
	}

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
		matWorld_ *= parent_->matWorld_;
	}
	// 定数バッファに転送する
	TransferMatrix();
}


void WorldTransform::BillboardUpdateMatrix(const ViewProjection& viewProjection, const BillboardType& billboardAxis,const AdaptRotate& adaptRotate) {
	// スケール、回転、平行移動行列を計算
	Matrix4x4 scaleMatrix = MakeScaleMatrix(scale_);
	Matrix4x4 translateMatrix = MakeTranslateMatrix(translation_);

	// カメラ行列を取得
	Matrix4x4 cameraMatrix = viewProjection.GetCameraMatrix();
	backToFrontMatrix_ = MakeRotateYMatrix(std::numbers::pi_v<float>); // 視点を反転

	// カメラ位置を取得し、オブジェクトとのベクトルを計算
	Vector3 cameraPos = viewProjection.GetWorldPos();
	Vector3 toCamera = { cameraPos.x - translation_.x, 0.0f, cameraPos.z - translation_.z };
	toCamera = (toCamera).Normalize();

	// ビルボード行列の計算
	switch (billboardAxis) {
	case BillboardType::XYZ:
		// 完全なビルボード（カメラの回転を全適用）
		billboardMatrix_ = backToFrontMatrix_ * cameraMatrix;

		break;

	case BillboardType::Y: {
		// Y軸ビルボード（XZ平面でカメラの方向を向く）
		float angleY = std::atan2(toCamera.x, toCamera.z);
		billboardMatrix_ = MakeRotateYMatrix(angleY);

		break;
	}

	default:
		// 何もしない
		billboardMatrix_ = MakeIdentity4x4();
		break;
	}

	if (!adaptRotate.isX_) { rotation_.x = 0.0f; }
	if (!adaptRotate.isY_) { rotation_.y = 0.0f; }
	if (!adaptRotate.isZ_) { rotation_.z = 0.0f; }

	// X/Z軸の回転を適用
	Matrix4x4 xzRotationMatrix = MakeRotateMatrix(rotation_);

	// ビルボード行列に適用（Y軸回転 → X/Z軸回転の順）
	billboardMatrix_ = xzRotationMatrix* billboardMatrix_;

	// ビルボード行列の平行移動成分をクリア
	billboardMatrix_.m[3][0] = 0.0f;
	billboardMatrix_.m[3][1] = 0.0f;
	billboardMatrix_.m[3][2] = 0.0f;

	// 最終的なワールド行列を計算
	matWorld_ = scaleMatrix * billboardMatrix_ * translateMatrix;

	// 親子関係があれば親のワールド行列を掛ける
	if (parent_) {
		matWorld_ *= parent_->matWorld_;
	}

	// 定数バッファに転送する
	TransferMatrix();
}



void WorldTransform::SetParent(const WorldTransform* parent) {
	parent_ = parent;
}



Vector3 WorldTransform::LookAt(const Vector3& direction) const {
	// 引数の方向ベクトルを正規化
	Vector3 normalizedDirection = Vector3::Normalize(direction);

	// 現在の行列を基にワールド方向を計算
	Matrix4x4 rotateMatrix = MakeRotateMatrix(rotation_);
	Vector3 worldDirection = TransformNormal(normalizedDirection, rotateMatrix);

	// 正規化して返す
	return Vector3::Normalize(worldDirection);
}

///=====================================================
/// WorldPos取得
///=====================================================
Vector3 WorldTransform::GetWorldPos() const {

	return Vector3(
		matWorld_.m[3][0], // X成分
		matWorld_.m[3][1], // Y成分
		matWorld_.m[3][2]  // Z成分
	);
}


///=====================================================
/// ローカル座標取得
///=====================================================
Vector3 WorldTransform::GetLocalPos() const {
	// 親が設定されていない場合はワールド座標をそのまま返す
	if (parent_ == nullptr) {
		return GetWorldPos();
	}

	// 親のワールド行列の逆行列を計算
	Matrix4x4 parentInverse = Inverse(parent_->matWorld_);

	// ワールド座標を取得
	Vector3 worldPos = GetWorldPos();

	// 親の逆行列を使ってローカル座標を計算
	Vector3 localPos = MatrixTransform(worldPos, parentInverse);
	return localPos;
}
