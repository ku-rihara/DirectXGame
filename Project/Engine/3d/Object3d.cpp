#include"Object3d.h"
#include"ModelManager.h"

//#include"struct/ModelData.h"

Object3d::Object3d() {

}
Object3d::~Object3d() {

}

Object3d* Object3d::CreateModel(const std::string& instanceName, const std::string& extension) {
	// 新しいModelインスタンスを作成
	Object3d* object3d=new Object3d();
	ModelManager::GetInstance()->LoadModel(instanceName, extension);
	object3d->SetModel(instanceName, extension);
	object3d->CreateWVPResource();
	object3d->CreateMaterialResource();
	//object3d->transform_.Init();
	return object3d;
}

/// 初期化
void Object3d::Init() {
	/// WVPリソース作成
	CreateWVPResource();
	
}

//更新
void Object3d::Update() {
	color_.TransferMatrix();
	/*transform_.UpdateMatrix();*/
}

//描画
void Object3d::Draw(const WorldTransform& worldTransform,const ViewProjection& viewProjection, std::optional<uint32_t> textureHandle) {
	if (model_) {
		// WVP行列の計算
		if (model_->GetIsFileGltf()) {//.gltfファイルの場合
			wvpDate_->WVP = model_->GetModelData().rootNode.localMatrix * worldTransform.matWorld_ * viewProjection.matView_ * viewProjection.matProjection_;
			wvpDate_->WorldInverseTranspose = Inverse(Transpose(model_->GetModelData().rootNode.localMatrix * wvpDate_->World));
		}
		else {//.objファイルの場合
			wvpDate_->WVP = worldTransform.matWorld_ * viewProjection.matView_ * viewProjection.matProjection_;
			wvpDate_->WorldInverseTranspose = Inverse(Transpose(wvpDate_->World));
		}
		model_->Draw(wvpResource_, material_, textureHandle);
	}
}

void Object3d::DebugImgui() {

	BaseObject3d::DebugImgui();
}

void Object3d::CreateWVPResource() {
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


void  Object3d::CreateMaterialResource() {
	BaseObject3d::CreateMaterialResource();
}