#include "BasePlayerHand.h"
/// imgui
#include<imgui.h>



///=========================================================
///　初期化
///==========================================================
void BasePlayerHand::Init() {


}

///=========================================================
///　更新
///==========================================================
void BasePlayerHand::Update() {
	
	BaseObject::Update();
}

///=========================================================
///　描画
///==========================================================
void BasePlayerHand::Draw(const ViewProjection& viewProjection) {
	BaseObject::Draw(viewProjection);
}
