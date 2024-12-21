#include "PlayerHandLeft.h"
/// imgui
#include<imgui.h>


PlayerHandLeft::PlayerHandLeft() {}

///=========================================================
///　初期化
///==========================================================
void PlayerHandLeft::Init() {

	BaseObject::Init();	// 基底クラスの初期化 
	BaseObject::CreateModel("cube", ".obj");/// モデルセット

	BasePlayerHand();
}

///=========================================================
///　更新
///==========================================================
void PlayerHandLeft::Update() {
	
	BasePlayerHand::Update();

}

///=========================================================
///　描画
///==========================================================
void PlayerHandLeft::Draw(const ViewProjection& viewProjection) {
	BasePlayerHand::Draw(viewProjection);
	
}
