
#include"ComboCountUP.h"
#include"Combo/ComboUIController.h"
/// frame
#include"Frame/Frame.h"
/// inupt
#include"JoyState/JoyState.h"
/// imgui
#include<imgui.h>

//初期化
ComboCountUP::ComboCountUP(ComboUIController* uiController)
    :BaseComboUIBehavior("ComboCountUP", uiController) {

  
}

ComboCountUP ::~ComboCountUP() {

}

//更新
void ComboCountUP::Update() {

}

void ComboCountUP::Debug() {
	ImGui::Text("ComboCountUP");
}

