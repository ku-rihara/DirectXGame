#include "SkyBox.h"
#include "assert.h"
#include"Lighrt/Light.h"
#include<imgui.h>

SkyBox::SkyBox() {}

SkyBox::~SkyBox() {}

void SkyBox::Init() {
    primitiveBox_ = std::make_unique<PrimitiveBox>();
    primitiveBox_->Init();
    primitiveBox_->SetTexture("Resources/Texture/output.dds");
	transform_.Init();

	transform_.scale_ = {300, 300, 300};

}

void SkyBox::Update() {
    
	transform_.UpdateMatrix();
}

void SkyBox::Draw(ViewProjection& viewProjection) {
    primitiveBox_->Draw(transform_, viewProjection);
}

void SkyBox::Debug() {
#ifdef _DEBUG
	if (ImGui::CollapsingHeader("SkyBox")) {
		ImGui::PushID("SkyBox");
		ImGui::DragFloat3("Position", &transform_.translation_.x, 0.1f);
		ImGui::DragFloat3("Scale", &transform_.scale_.x, 0.1f);
		ImGui::PopID();
	}
#endif // _DEBUG
}