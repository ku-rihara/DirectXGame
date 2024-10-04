#include"BaseObject3d.h"
#include"DirectXCommon.h"


//void BaseObject3d::Update() {
//
//}

//void BaseObject3d::Draw() {
//
//}

void BaseObject3d::DebugImgui() {
	model_->DebugImGui();
}

void BaseObject3d::CreateWVPResource() {
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