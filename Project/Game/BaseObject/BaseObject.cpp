#include "BaseObject.h"


void BaseObject::Init() {

	/// ワールドトランスフォームの初期化
	transform_.Initialize();
	/*aabb_.min = { 0,0,0 };
	aabb_.max = { 1,1,1 };*/
	//カラーのセット
	objColor_.Initialize();
	objColor_.SetColor(Vector4(1, 1, 1, 1));
}

void BaseObject::Update() {

	//元となるワールドトランスフォームの更新
	transform_.UpdateMatrix();
	/// 色転送
	objColor_.TransferMatrix();
}

void BaseObject::Draw(const ViewProjection& viewProjection) {
	obj3d_->Draw(transform_, viewProjection, &objColor_);
}

Vector3 BaseObject::GetWorldPosition() const {
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得
	worldPos.x = transform_.matWorld_.m[3][0];
	worldPos.y = transform_.matWorld_.m[3][1];
	worldPos.z = transform_.matWorld_.m[3][2];

	return worldPos;
}

void BaseObject::CreateModel(const std::string modelname) {
	obj3d_ = std::make_unique<Object3d>();
	obj3d_->Initialize(modelname);
}


