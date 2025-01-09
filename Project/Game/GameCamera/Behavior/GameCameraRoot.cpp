/// behavior
#include"GameCameraRoot.h"
/// boss
#include"GameCamera/GameCamera.h"
/// frame
#include"Frame/Frame.h"
#include<imgui.h>


//初期化
GameCameraRoot::GameCameraRoot(GameCamera* gameCamera)
	: BaseGameCameraBehavior("GameCameraRoot", gameCamera) {

	/// ===================================================
	///変数初期化
	/// ===================================================
	
}

GameCameraRoot ::~GameCameraRoot() {

}

//更新
void GameCameraRoot::Update() {
	
}


void  GameCameraRoot::Debug() {
	ImGui::Text("GameCameraRoot");
}