#include "PlayerHandRight.h"
/// imgui
#include<imgui.h>


PlayerHandRight::PlayerHandRight() {}

///=========================================================
///　初期化
///==========================================================
void PlayerHandRight::Init() {

	BaseObject::Init();	// 基底クラスの初期化 
	BaseObject::CreateModel("cube", ".obj");/// モデルセット

	BasePlayerHand();
	
}

///=========================================================
///　更新
///==========================================================
void PlayerHandRight::Update() {
	
	BasePlayerHand::Update();
}

///=========================================================
///　描画
///==========================================================
void PlayerHandRight::Draw(const ViewProjection& viewProjection) {
	BasePlayerHand::Draw(viewProjection);
	
}
