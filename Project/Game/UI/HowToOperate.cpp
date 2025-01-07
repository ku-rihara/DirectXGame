#include "HowToOperate.h"
#include"base/TextureManager.h"
#include<imgui.h>


void HowToOperate::Init() {
	uint32_t handle=TextureManager::GetInstance()->LoadTexture("Resources/Texture/HowToOperate.png");
	
	sprite_.reset(Sprite::Create(handle, {}, { 1,1,1,1 }));
	sprite_->anchorPoint_ = { 0.5f,0.5f };
	position_ = { 50,50 };
}

void HowToOperate::Update() {
	sprite_->SetPosition(position_);	
}

void HowToOperate::Draw() {
	
	sprite_->Draw();
}

void HowToOperate::Debug() {
	ImGui::SeparatorText("SpriteText");
	ImGui::DragFloat2("howtoope", &position_.x, 0.1f);
}