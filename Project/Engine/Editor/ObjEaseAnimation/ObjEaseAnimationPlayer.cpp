#include "ObjEaseAnimationPlayer.h"

using namespace KetaEngine;

void ObjEaseAnimationPlayer::Init() {
    BaseEffectPlayer::Init();
}

void ObjEaseAnimationPlayer::Update(float speedRate) {
    if (effectData_) {
        effectData_->Update(speedRate);
    }

}

void ObjEaseAnimationPlayer::Play(const std::string& animationName, const std::string& categoryName) {
    if (effectData_) {
        effectData_->Pause();
    }

    effectData_.reset();
    effectData_ = CreateEffectData();

    // キャスト
    auto* animeData = dynamic_cast<ObjEaseAnimationData*>(effectData_.get());
    if (animeData) {
        animeData->Init(animationName, categoryName);
        animeData->LoadData();
        animeData->Play();

        // アニメーション名単位のコールバックを適用
        auto it = namedLoopEndCallbacks_.find(animationName);
        if (it != namedLoopEndCallbacks_.end()) {
            animeData->SetLoopEndCallback(it->second);
        }
    }

    currentCategoryName_ = categoryName;
    currentEffectName_   = animationName;
}

std::unique_ptr<BaseEffectData> ObjEaseAnimationPlayer::CreateEffectData() {
    return std::make_unique<ObjEaseAnimationData>();
}

Vector3 ObjEaseAnimationPlayer::GetCurrentScale() const {
    auto* animeData = dynamic_cast<ObjEaseAnimationData*>(effectData_.get());
    if (animeData) {
        return animeData->GetActiveKeyFrameValue(ObjEaseAnimationData::TransformType::Scale);
    }
    return Vector3::OneVector();
}

Vector3 ObjEaseAnimationPlayer::GetCurrentRotation() const {
    auto* animeData = dynamic_cast<ObjEaseAnimationData*>(effectData_.get());
    if (animeData) {
        return animeData->GetActiveKeyFrameValue(ObjEaseAnimationData::TransformType::Rotation);
    }
    return Vector3::ZeroVector();
}

Vector3 ObjEaseAnimationPlayer::GetCurrentTranslation() const {
    auto* animeData = dynamic_cast<ObjEaseAnimationData*>(effectData_.get());
    if (animeData) {
        return animeData->GetActiveKeyFrameValue(ObjEaseAnimationData::TransformType::Translation);
    }
    return Vector3::ZeroVector();
}

Vector3 ObjEaseAnimationPlayer::GetCurrentScaleAnchor() const {
    auto* animeData = dynamic_cast<ObjEaseAnimationData*>(effectData_.get());
    if (animeData) {
        return animeData->GetActiveAnchorValue(ObjEaseAnimationData::TransformType::Scale);
    }
    return Vector3::ZeroVector();
}

Vector3 ObjEaseAnimationPlayer::GetCurrentRotationAnchor() const {
    auto* animeData = dynamic_cast<ObjEaseAnimationData*>(effectData_.get());
    if (animeData) {
        return animeData->GetActiveAnchorValue(ObjEaseAnimationData::TransformType::Rotation);
    }
    return Vector3::ZeroVector();
}

Vector3 ObjEaseAnimationPlayer::GetCurrentTranslationAnchor() const {
    auto* animeData = dynamic_cast<ObjEaseAnimationData*>(effectData_.get());
    if (animeData) {
        return animeData->GetActiveAnchorValue(ObjEaseAnimationData::TransformType::Translation);
    }
    return Vector3::ZeroVector();
}

ObjEaseAnimationData* ObjEaseAnimationPlayer::GetAnimationData() {
    return dynamic_cast<ObjEaseAnimationData*>(effectData_.get());
}

bool ObjEaseAnimationPlayer::IsLookingAtDirection() const {
    auto* animeData = dynamic_cast<ObjEaseAnimationData*>(effectData_.get());
    if (animeData) {
        return animeData->IsLookingAtDirection();
    }
    return false;
}

Vector3 ObjEaseAnimationPlayer::GetMovementDirection() const {
    auto* animeData = dynamic_cast<ObjEaseAnimationData*>(effectData_.get());
    if (animeData) {
        return animeData->GetMovementDirection();
    }
    return Vector3::ToForward();
}

bool ObjEaseAnimationPlayer::IsTranslationReturning() const {
    auto* animeData = dynamic_cast<ObjEaseAnimationData*>(effectData_.get());
    if (animeData) {
        return animeData->IsTranslationReturning();
    }
    return false;
}

void ObjEaseAnimationPlayer::SetLoop(bool isLoop) {
    auto* animeData = GetAnimationData();
    if (animeData) {
        animeData->SetLoop(isLoop);
    }
}

void ObjEaseAnimationPlayer::SetLoopEndCallback(const std::string& animationName, std::function<void()> callback) {
    namedLoopEndCallbacks_[animationName] = std::move(callback);

    // 現在そのアニメーションが再生中なら即座に適用
    if (currentEffectName_ == animationName) {
        auto* animeData = GetAnimationData();
        if (animeData) {
            animeData->SetLoopEndCallback(namedLoopEndCallbacks_[animationName]);
        }
    }
}

void ObjEaseAnimationPlayer::SetLoopEndCallback(const std::function<void()>& callback) {
    auto* animeData = GetAnimationData();
    if (animeData) {
        animeData->SetLoopEndCallback(callback);
    }
}

void ObjEaseAnimationPlayer::SetPreAnimationOffsetsToOriginalValues() {
    auto* animeData = GetAnimationData();
    if (animeData) {
        animeData->SetPreAnimationOffsetsToOriginalValues();
    }
}
