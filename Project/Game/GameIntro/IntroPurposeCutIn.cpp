#include "IntroPurposeCutIn.h"
#include "Frame/Frame.h"
#include <imgui.h>

void IntroPurposeCutIn::Init(const std::string& name) {

    BaseGameIntro::Init(name);

    // スプライト初期化
    for (size_t i = 0; i < sprites_.size(); ++i) {
        sprites_[i].reset(KetaEngine::Sprite::Create("PurposeUI/gamePurposeNo" + std::to_string(i + 1) + ".dds"));
        sprites_[i]->transform_.scale = Vector2::ZeroVector();
    }

    backLineSprite_.reset(KetaEngine::Sprite::Create("PurposeUI/gamePurposeLine.dds"));
    backLineSprite_->transform_.scale = Vector2::ZeroVector();

    // Easing 初期化
    EasingInit();

    // 変数初期化
    step_             = Step::SideAppearWait;
    isFinish_         = false;
    isAbleEnemySpawn_ = false;
    currentTime_      = 0.0f;
}

void IntroPurposeCutIn::Update(float playSpeed) {
    BaseGameIntro::Update(playSpeed);
    (this->*spFuncTable_[static_cast<size_t>(step_)])();
}


// Step function
void IntroPurposeCutIn::SideAppearWait() {
    ProcessStep(sideAppearWaitTime_, Step::SideAppear);
}
void IntroPurposeCutIn::SideAppear() {

    // 　サイドUIのAppear演出
    for (size_t i = 0; i < spriteVariable_.sideAppearEase.size(); ++i) {
        spriteVariable_.sideAppearEase[i]->Update(KetaEngine::Frame::DeltaTime());
        sprites_[i]->transform_.pos.x = spriteVariable_.sideAppearPosX[i];
    }

    // 　ScaleY
    spriteVariable_.appearScaleEase->Update(KetaEngine::Frame::DeltaTime());
    for (size_t i = 0; i < spriteVariable_.sideAppearEase.size(); ++i) {
        sprites_[i]->transform_.scale = spriteVariable_.appearScale;
    }
    backLineSprite_->transform_.scale = spriteVariable_.appearScale;

    if (!spriteVariable_.isBackSideUI) {
        return;
    }

    step_ = Step::CenterAppearWait;
}

void IntroPurposeCutIn::CenterAppearWait() {
    ProcessStep(centerAppearWaitTime_, Step::CenterAppear);
}
void IntroPurposeCutIn::CenterAppear() {
    // 　サイドUIのBack演出
    for (size_t i = 0; i < spriteVariable_.sideBackEase.size(); ++i) {
        spriteVariable_.sideBackEase[i]->Update(KetaEngine::Frame::DeltaTime());
        sprites_[i]->transform_.pos.x = spriteVariable_.sideBackPosX[i];
    }

    // centerScale演出
    spriteVariable_.centerAppearEase->Update(KetaEngine::Frame::DeltaTime());
    sprites_[CENTER]->transform_.scale = spriteVariable_.centerScale;

    // 次のステップ
    if (spriteVariable_.centerAppearEase->IsFinished() && spriteVariable_.sideBackEase[LEFT]->IsFinished()) {
        step_ = Step::CloseWait;
    }
}

void IntroPurposeCutIn::CloseWait() {
    ProcessStep(closeWaitTime_, Step::Close);
}

void IntroPurposeCutIn::Close() {

    // 　ScaleY
    spriteVariable_.closeScaleEase->Update(KetaEngine::Frame::DeltaTime());
    for (size_t i = 0; i < sprites_.size(); ++i) {
        sprites_[i]->transform_.scale = spriteVariable_.closeScale;
    }
    backLineSprite_->transform_.scale = spriteVariable_.closeScale;
}

void IntroPurposeCutIn::FinishWait() {
}
void IntroPurposeCutIn::Finish() {
}

bool IntroPurposeCutIn::ProcessStep(float limitTime, const Step& nextStep, const bool& enableEnemySpawn) {
    // タイム加算
    currentTime_ += playSpeed_;

    if (currentTime_ >= limitTime) {
        currentTime_ = 0.0f;
        step_        = nextStep;
        if (enableEnemySpawn) {
            isAbleEnemySpawn_ = true;
        }
        return true;
    }
    return false;
}

void IntroPurposeCutIn::RegisterParams() {
    BaseGameIntro::RegisterParams();

    globalParameter_->Regist(groupName_, "AppearWaitTime", &sideAppearWaitTime_);
    globalParameter_->Regist(groupName_, "closeWaitTime", &closeWaitTime_);
    globalParameter_->Regist(groupName_, "centerAppearWaitTime", &centerAppearWaitTime_);
    globalParameter_->Regist(groupName_, "finishWaitTime", &finishWaitTime_);
}

void IntroPurposeCutIn::AdjustParam() {
    BaseGameIntro::AdjustParam();
}

void IntroPurposeCutIn::AdjustUniqueParam() {
    ImGui::DragFloat("SideAppearWait Time", &sideAppearWaitTime_, 0.01f, 0.0f);
    ImGui::DragFloat("CenterAppearWait Time", &centerAppearWaitTime_, 0.01f, 0.0f);
    ImGui::DragFloat("CloseWait Time", &closeWaitTime_, 0.01f, 0.0f);
    ImGui::DragFloat("FinishWait Time", &finishWaitTime_, 0.01f, 0.0f);
}

void IntroPurposeCutIn::EasingInit() {

    // 生成
    for (size_t i = 0; i < spriteVariable_.sideAppearEase.size(); ++i) {
        spriteVariable_.sideAppearEase[i] = std::make_unique<KetaEngine::Easing<float>>();
        spriteVariable_.sideBackEase[i]   = std::make_unique<KetaEngine::Easing<float>>();
    }

    spriteVariable_.centerAppearEase = std::make_unique<KetaEngine::Easing<Vector2>>();
    spriteVariable_.appearScaleEase  = std::make_unique<KetaEngine::Easing<Vector2>>();
    spriteVariable_.closeScaleEase   = std::make_unique<KetaEngine::Easing<Vector2>>();

    // サイドUI出現Easing
    spriteVariable_.sideAppearEase[LEFT]->Init("leftAppearPosX.json");
    spriteVariable_.sideAppearEase[RIGHT]->Init("RightAppearPosX.json");

    // 元に戻るサイドUIEasing
    spriteVariable_.sideBackEase[LEFT]->Init("LeftBackPosX.json");
    spriteVariable_.sideBackEase[RIGHT]->Init("RightBackPosX.json");

    // CenterEasing
    spriteVariable_.centerAppearEase->Init("CenterAppearScale.json");

    // AppearScaleY
    spriteVariable_.appearScaleEase->Init("PurposeScaleXY.json");

    // CloseScaleY
    spriteVariable_.closeScaleEase->Init("PurposeCloseScaleXY.json");

    // 適応値、スタート値セット(サイドUI出現Easing)
    for (size_t i = 0; i < static_cast<size_t>(spriteVariable_.sideAppearEase.size()); ++i) {
        spriteVariable_.sideAppearEase[i]->SetAdaptValue(&spriteVariable_.sideAppearPosX[i]);
        spriteVariable_.sideAppearEase[i]->SetStartValue(sprites_[i]->GetStartParameter().position_.x);
        spriteVariable_.sideAppearEase[i]->Reset();
        spriteVariable_.sideAppearEase[i]->SetOnFinishCallback([this] {
            spriteVariable_.isBackSideUI = true;
        });
    }

    // 適応値、スタート値セット(元に戻るサイドUIEasing)
    for (size_t i = 0; i < static_cast<size_t>(spriteVariable_.sideBackEase.size()); ++i) {
        spriteVariable_.sideBackEase[i]->SetAdaptValue(&spriteVariable_.sideBackPosX[i]);
        spriteVariable_.sideBackEase[i]->SetStartValue(spriteVariable_.sideAppearEase[i]->GetEndValue());
        spriteVariable_.sideBackEase[i]->SetEndValue(sprites_[i]->GetStartParameter().position_.x);
        spriteVariable_.sideBackEase[i]->Reset();
    }

    // 適応値、スタート値セット(CenterEasing)
    spriteVariable_.centerAppearEase->SetAdaptValue(&spriteVariable_.centerScale);
    spriteVariable_.centerAppearEase->SetStartValue(Vector2::ZeroVector());
    spriteVariable_.centerAppearEase->Reset();

    // 適応値、スタート値セット(AppearScaleEaseY)
    spriteVariable_.appearScaleEase->SetAdaptValue(&spriteVariable_.appearScale);
    spriteVariable_.appearScaleEase->Reset();

    // 適応値、スタート値セット(CloseScaleEaseY)
    spriteVariable_.closeScaleEase->SetAdaptValue(&spriteVariable_.closeScale);
    spriteVariable_.closeScaleEase->Reset();
    spriteVariable_.closeScaleEase->SetOnFinishCallback([this] {
        isFinish_ = true;
    });
}

void (IntroPurposeCutIn::* IntroPurposeCutIn::spFuncTable_[])() = {
    &IntroPurposeCutIn::SideAppearWait,
    &IntroPurposeCutIn::SideAppear,
    &IntroPurposeCutIn::CenterAppearWait,
    &IntroPurposeCutIn::CenterAppear,
    &IntroPurposeCutIn::CloseWait,
    &IntroPurposeCutIn::Close,
    &IntroPurposeCutIn::FinishWait,
    &IntroPurposeCutIn::Finish,
};
