/// behavior
#include"ShakeWait.h"
/// boss
#include"GameCamera/GameCamera.h"
/// frame
#include"Frame/Frame.h"
#include<imgui.h>


//初期化
ShakeWait::ShakeWait(GameCamera* gameCamera)
	: BaseShakeState("ShakeWait", gameCamera) {

	/// ===================================================
	/// 変数初期化
	/// ===================================================
	
}

ShakeWait ::~ShakeWait() {

}

//更新
void ShakeWait::Update() {
	
}


void  ShakeWait::Debug() {
	ImGui::Text("ShakeWait");
}