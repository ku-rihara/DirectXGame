#include"Object3d.h"

Object3d* Object3d::CreateModel(const std::string& instanceName) {
	// 新しいModelインスタンスを作成
	Object3d* object3d = new Object3d();
	object3d->model_.reset(Model::Create(instanceName));
	object3d->CreateWVPResource();
	object3d->transform_.Init();
	return object3d;
}

//更新
void Object3d::Update() {
	transform_.UpdateMatrix();
}

//描画
void Object3d::Draw( const ViewProjection& viewProjection, std::optional<uint32_t> textureHandle, const Vector4& color) {
	// WVP行列の計算
	wvpDate_->WVP = transform_.matWorld_ * viewProjection.matView_ * viewProjection.matProjection_;
	wvpDate_->WorldInverseTranspose = Inverse(Transpose(wvpDate_->World));
	
	if (model_) {
		model_->Draw( wvpResource_, textureHandle, color);
	}
}

void Object3d::DebugImgui() {
	BaseObject3d::DebugImgui();
}

void Object3d::CreateWVPResource() {
	//行列--------------------------------------------------------------------------------------------------------
	wvpResource_ = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(TransformationMatrix));
	//データを書き込む
	wvpDate_ = nullptr;
	//書き込むためのアドレスを取得
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpDate_));
	//単位行列を書き込んでおく
	wvpDate_->WVP = MakeIdentity4x4();
	wvpDate_->World = MakeIdentity4x4();
	wvpDate_->WorldInverseTranspose = MakeIdentity4x4();
}