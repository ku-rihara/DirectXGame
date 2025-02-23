/// behavior
#include"ShakeStateRoot.h"
/// boss
#include"GameCamera/GameCamera.h"
/// frame
#include"Frame/Frame.h"
#include<imgui.h>


//初期化
ShakeStateRoot::ShakeStateRoot(GameCamera* gameCamera)
	: BaseShakeState("ShakeStateRoot", gameCamera) {

	/// ===================================================
	/// 変数初期化
	/// ===================================================
	
}

ShakeStateRoot ::~ShakeStateRoot() {

}

//更新
void ShakeStateRoot::Update() {
	
}


void  ShakeStateRoot::Debug() {
	ImGui::Text("ShakeStateRoot");
}