#include"Object3d.h"

Object3d* Object3d::CreateModel(const std::string& instanceName) {
	// 新しいModelインスタンスを作成
	Object3d* object3d = new Object3d();
	object3d->model_.reset(Model::Create(instanceName));
	object3d->CreateWVPResource();
	object3d->worldTransform_.Init();
	return object3d;
}


void Object3d::Update() {
	worldTransform_.UpdateMatrix();
}

void Object3d::Draw( const ViewProjection& viewProjection, std::optional<uint32_t> textureHandle, const Vector4& color) {
	// WVP行列の計算
	wvpDate_->WVP = worldTransform_.matWorld_ * viewProjection.matView_ * viewProjection.matProjection_;
	wvpDate_->WorldInverseTranspose = Inverse(Transpose(wvpDate_->World));
	
	if (model_) {
		model_->Draw( wvpResource_, textureHandle, color);
	}

}

void Object3d::DebugImgui() {
	BaseObject3d::DebugImgui();
}

void Object3d::CreateWVPResource() {
	BaseObject3d::CreateWVPResource();
}