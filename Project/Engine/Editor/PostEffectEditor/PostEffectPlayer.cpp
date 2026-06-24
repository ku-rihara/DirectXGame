#include "PostEffectPlayer.h"

using namespace KetaEngine;
#include "PostEffectData.h"
#include "PostEffect/PostEffectRenderer.h"

///============================================================
/// 初期化
///============================================================
void PostEffectPlayer::Init() {
    BaseEffectPlayer::Init();
}

///============================================================
/// 更新
///============================================================
void PostEffectPlayer::Update(float speedRate) {
    if (!effectData_) {
        return;
    }
    effectData_->Update(speedRate);
}

///============================================================
/// Play
///============================================================
void PostEffectPlayer::Play(const std::string& presetName, const std::string& categoryName) {
    effectData_.reset();
    effectData_ = CreateEffectData();

    if (effectData_) {
        effectData_->Init(presetName, categoryName);
        effectData_->LoadData();
        effectData_->Play();
        currentEffectName_ = presetName;
    }
}

///============================================================
/// StopAndReset
///============================================================
void PostEffectPlayer::StopAndReset() {
    if (effectData_) {
        effectData_->Reset();
    }
    PostEffectRenderer::GetInstance()->SetPostEffectMode(PostEffectMode::NONE);
}

///============================================================
/// CreateEffectData
///============================================================
std::unique_ptr<BaseEffectData> PostEffectPlayer::CreateEffectData() {
    return std::make_unique<PostEffectData>();
}

///============================================================
/// GetData
///============================================================
PostEffectData* PostEffectPlayer::GetData() const {
    return static_cast<PostEffectData*>(effectData_.get());
}
