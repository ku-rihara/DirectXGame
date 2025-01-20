#include "TitleFont.h"
#include"Frame/Frame.h"
#include"base/TextureManager.h"
#include<imgui.h>


void TitleFont::Init() {
	uint32_t phandle=TextureManager::GetInstance()->LoadTexture("Resources/Texture/TitleFontPunch.png");
	uint32_t rhandle = TextureManager::GetInstance()->LoadTexture("Resources/Texture/TitleFontRush.png");
	uint32_t bhandle = TextureManager::GetInstance()->LoadTexture("Resources/Texture/StartBottomA.png");
	
	punchSprite_.reset(Sprite::Create(phandle, {}, { 1,1,1,1 }));
	punchSprite_->anchorPoint_ = { 0.5f,0.5f };

	rushSprite_.reset(Sprite::Create(rhandle, {}, { 1,1,1,1 }));
	rushSprite_->anchorPoint_ = { 0.5f,0.5f };

	bottomSprite_.reset(Sprite::Create(bhandle, {}, { 1,1,1,1 }));
	bottomSprite_->anchorPoint_ = { 0.5f,0.5f };

	punchPosition_ = { 696.0f,137.9f };
	rushPosition_ = { 1066.0f,135.6f };
	bottonPosition_ = { 933,540 };

	punchSprite_->SetScale(Vector2(0, 0));
	rushSprite_->SetScale(Vector2(0, 0));
	bottomSprite_->SetScale(Vector2(0, 0));

	punchEase_.maxTime = 0.7f;
	rushEase_.maxTime = 0.7f;
	bottomEase_.maxTime = 0.7f;

	initScale_ = { 1.0f,1.0f };

	step_ = Step::PUNCH;
}

void TitleFont::Update() {
	switch (step_)
	{
	case TitleFont::Step::PUNCH:
		///----------------------------------------------------
		/// パンチ
		///----------------------------------------------------
		punchEase_.time += Frame::DeltaTime();
		punchSprite_->SetScale(EaseOutBack(Vector2(0,0),initScale_,punchEase_.time,punchEase_.maxTime));

		if (punchEase_.time < punchEase_.maxTime)break;
		punchEase_.time = punchEase_.maxTime;
		punchSprite_->SetScale(initScale_);
		step_ = Step::RUSH;
		
		break;
	case TitleFont::Step::RUSH:
		///----------------------------------------------------
		/// ラッシュ
		///----------------------------------------------------
		rushEase_.time += Frame::DeltaTime();
		rushSprite_->SetScale(EaseOutBack(Vector2(0, 0), initScale_, rushEase_.time, rushEase_.maxTime));

		if (rushEase_.time < rushEase_.maxTime)break;
		rushEase_.time = rushEase_.maxTime;
		rushSprite_->SetScale(initScale_);
		step_ = Step::BOTTON;
		break;
	case TitleFont::Step::BOTTON:
		///----------------------------------------------------
		/// ボタン
		///----------------------------------------------------
		bottomEase_.time += Frame::DeltaTime();
		bottomSprite_->SetScale(EaseOutBack(Vector2(0, 0), initScale_, bottomEase_.time, bottomEase_.maxTime));

		if (bottomEase_.time < bottomEase_.maxTime)break;
		bottomEase_.time = bottomEase_.maxTime;
		bottomSprite_->SetScale(initScale_);
		step_ = Step::BOTTONPOST;
		break;
	case TitleFont::Step::BOTTONPOST:
		///----------------------------------------------------
		/// パンチ
		///----------------------------------------------------
		break;
	default:
		break;
	}

	punchSprite_->SetPosition(punchPosition_);
	rushSprite_->SetPosition(rushPosition_);
	bottomSprite_->SetPosition(bottonPosition_);
}

void TitleFont::Draw() {
	
	punchSprite_->Draw();
	rushSprite_->Draw();
	bottomSprite_->Draw();
}

void TitleFont::Debug() {
	ImGui::SeparatorText("SpriteText");
	ImGui::DragFloat2("punchPosition_", &punchPosition_.x, 0.1f);
	ImGui::DragFloat2("rushPosition_", &rushPosition_.x, 0.1f);
	ImGui::DragFloat2("bottonPosition_", &bottonPosition_.x, 0.1f);
}