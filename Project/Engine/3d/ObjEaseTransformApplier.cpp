#include "ObjEaseTransformApplier.h"
#include "Editor/ObjEaseAnimation/ObjEaseAnimationData.h"
#include "Editor/ObjEaseAnimation/ObjEaseAnimationPlayer.h"
#include "WorldTransform.h"
#include <cmath>

using namespace KetaEngine;

ObjEaseTransformApplier::ObjEaseTransformApplier()  = default;
ObjEaseTransformApplier::~ObjEaseTransformApplier() = default;

void ObjEaseTransformApplier::Init() {
    if (!player_) {
        player_ = std::make_unique<ObjEaseAnimationPlayer>();
        player_->Init();
    }
}

void ObjEaseTransformApplier::Play(
    const std::string& animationName,
    const std::string& categoryName,
    const Vector3& currentOffsetScale,
    const Vector3& currentOffsetRotation,
    const Vector3& currentOffsetTranslation,
    WorldTransform* railParent) {
    if (!player_) {
        player_ = std::make_unique<ObjEaseAnimationPlayer>();
        player_->Init();
    }

    applyOriginalOnStop_ = false;
    player_->Play(animationName, categoryName);

    auto* animeData = player_->GetAnimationData();
    if (animeData) {
        animeData->SetPreAnimationOffsets(currentOffsetScale, currentOffsetRotation, currentOffsetTranslation);
    }

    // Rail使用時、親を設定
    if (animeData && animeData->GetIsUseRailActiveKeyFrame()) {
        auto* railPlayer = animeData->GetCurrentRailPlayer();
        if (railPlayer && railParent) {
            railPlayer->SetParent(railParent);
        }
    }
}

void ObjEaseTransformApplier::Stop() {
    if (player_) {
        player_->Stop();
    }
}

void ObjEaseTransformApplier::Update() {
    if (player_) {
        player_->Update();
    }
}

bool ObjEaseTransformApplier::Apply(
    OffsetResult& result,
    const Vector3& worldRotation,
    const Quaternion& quaternion,
    bool isAdaptDirectScale,
    bool isQuaternionMode) {
    if (!player_ || !player_->IsPlaying()) {
        lastPlayDirection_ = Vector3::ZeroVector();

        if (applyOriginalOnStop_) {
            auto* animeData = player_ ? player_->GetAnimationData() : nullptr;
            if (animeData) {
                result.scale       = animeData->GetOriginalValue(ObjEaseAnimationData::TransformType::Scale);
                result.rotation    = animeData->GetOriginalValue(ObjEaseAnimationData::TransformType::Rotation);
                result.translation = animeData->GetOriginalValue(ObjEaseAnimationData::TransformType::Translation);
            }
        }
        return false;
    }

    auto* animeData = player_->GetAnimationData();
    if (!animeData) {
        lastPlayDirection_ = Vector3::ZeroVector();
        return false;
    }

    // Scale
    if (isAdaptDirectScale) {
        result.overwriteDirectScale = true;
        result.directScaleValue     = player_->GetCurrentScale();
    } else {
        result.scale = player_->GetCurrentScale();
    }

    // Translation
    if (!animeData->GetIsUseRailActiveKeyFrame()) {
        Vector3 trans = player_->GetCurrentTranslation();
        // 対象の向いてる方向を進行方向として動く場合はローカル→ワールド変換
        if (animeData->IsTranslationAlongForward()) {
            Matrix4x4 rotMat = MakeRotateMatrix(worldRotation);
            trans            = TransformNormal(trans, rotMat);
        }
        result.translation = trans;
    } else {
        auto* railPlayer = animeData->GetCurrentRailPlayer();
        if (railPlayer) {
            result.translation = railPlayer->GetCurrentPosition();
        }
    }

    // 進行方向を向く設定が有効な場合は方向から回転を決定する
    if (lookAtDirectionEnabled_ && player_->IsLookingAtDirection()) {
        Vector3 dir = player_->GetMovementDirection();
        if (dir.Length() > 0.001f) {
            bool isGoingForward =
                (lastPlayDirection_.Length() < 0.001f) || (dir.Dot(lastPlayDirection_) >= 0.0f);
            Vector3 applyDir;
            if (isGoingForward) {
                applyDir           = dir;
                lastPlayDirection_ = dir;
            } else {
                applyDir           = -dir;
                lastPlayDirection_ = -dir;
            }
            ApplyLookAtDirection(result, applyDir, isQuaternionMode, quaternion);
        } else if (lastPlayDirection_.Length() > 0.001f) {
            ApplyLookAtDirection(result, lastPlayDirection_, isQuaternionMode, quaternion);
        }
    } else {
        // Rotationをオフセット
        if (isQuaternionMode) {
            Vector3 rotationOffset = player_->GetCurrentRotation();
            result.quaternion      = Quaternion::EulerToQuaternion(rotationOffset);
        } else {
            result.rotation = player_->GetCurrentRotation();
        }
    }

    // アニメーションセクションのアンカーポイントを適用
    result.anchorScale       = player_->GetCurrentScaleAnchor();
    result.anchorRotation    = player_->GetCurrentRotationAnchor();
    result.anchorTranslation = player_->GetCurrentTranslationAnchor();

    return true;
}

bool ObjEaseTransformApplier::ApplyOriginalValues(OffsetResult& result) {
    if (!player_)
        return false;
    auto* animeData = player_->GetAnimationData();
    if (!animeData)
        return false;

    result.scale       = animeData->GetOriginalValue(ObjEaseAnimationData::TransformType::Scale);
    result.rotation    = animeData->GetOriginalValue(ObjEaseAnimationData::TransformType::Rotation);
    result.translation = animeData->GetOriginalValue(ObjEaseAnimationData::TransformType::Translation);

    applyOriginalOnStop_ = true;
    return true;
}

void ObjEaseTransformApplier::ApplyLookAtDirection(
    OffsetResult& result,
    const Vector3& direction,
    bool isQuaternionMode,
    const Quaternion& quaternion) {
    if (direction.Length() < 0.001f) {
        return;
    }

    Vector3 normalizedDir = direction.Normalize();

    if (isQuaternionMode) {
        Vector3 forward          = Vector3::ToForward();
        Matrix4x4 rotationMatrix = DirectionToDirection(forward, normalizedDir);
        Quaternion lookAtQuat    = QuaternionFromMatrix(rotationMatrix);
        result.quaternion        = lookAtQuat * quaternion;
    } else {
        float rotateY          = std::atan2(normalizedDir.x, normalizedDir.z);
        float horizontalLength = std::sqrt(
            normalizedDir.x * normalizedDir.x + normalizedDir.z * normalizedDir.z);
        float rotateX     = std::atan2(-normalizedDir.y, horizontalLength);
        result.rotation.y = rotateY;
        result.rotation.x = rotateX;
    }
}

bool ObjEaseTransformApplier::IsPlaying() const {
    return player_ && player_->IsPlaying();
}

ObjEaseAnimationPlayer* ObjEaseTransformApplier::GetPlayer() {
    return player_.get();
}
