#include "ParticleUpdater.h"

using namespace KetaEngine;

#include "Base/TextureManager.h"
#include "Frame/Frame.h"
#include "MathFunction.h"
#include "Box.h"

void ParticleUpdater::UpdateDissolvePlayer(ParticleManager::ParticleGroup& group) {
    if (group.dissolvePlayer && group.dissolvePlayer->IsPlaying()) {
        group.dissolvePlayer->Update(Frame::DeltaTimeRate());
        group.material.GetMaterialData()->enableDissolve = 1;

        auto& dp          = group.dissolveParams;
        dp.startThreshold = group.dissolvePlayer->GetStartThreshold();
        dp.endThreshold   = group.dissolvePlayer->GetEndThreshold();
        dp.maxTime        = (group.dissolvePlayer->GetMaxTime() > 0.0f) ? group.dissolvePlayer->GetMaxTime() : 0.001f;
        dp.offsetTime     = group.dissolvePlayer->GetOffsetTime();
        dp.easeType       = group.dissolvePlayer->GetEaseType();
        dp.isActive       = true;

        const std::string& texPath = group.dissolvePlayer->GetTexturePath();
        if (!texPath.empty() && texPath != group.lastDissolveTexturePath) {
            group.dissolveTextureHandle   = TextureManager::GetInstance()->LoadTexture(texPath);
            group.lastDissolveTexturePath = texPath;
        }
    }
}

void ParticleUpdater::UpdateGroup(
    ParticleManager::ParticleGroup& group,
    const ParticleManager::AccelerationField& field,
    const ViewProjection* viewProjection) {

    ///------------------------------------------------------------------------
    /// Dissolveプレイヤー更新 (グループレベル)
    ///------------------------------------------------------------------------
    UpdateDissolvePlayer(group);

    std::list<ParticleManager::Particle>& particles = group.particles;

    /// 粒子一つ一つの更新
    for (auto it = particles.begin(); it != particles.end();) {

        ///------------------------------------------------------------------------
        /// 加速フィールド
        ///------------------------------------------------------------------------
        if (field.isAdaption && IsCollision(field.area, it->worldTransform_->translation_)) {
            it->velocity_ += field.acceleration * Frame::DeltaTimeRate();
        }

        ///------------------------------------------------------------------------
        /// スケール変更
        ///------------------------------------------------------------------------
        if ((*it).isAdaptEasing) {
            (*it).scaleEasing->Update(Frame::DeltaTimeRate());
        }

        ///------------------------------------------------------------------------
        /// Translateイージング更新
        ///------------------------------------------------------------------------
        if (it->isAdaptTranslateEasing) {
            it->translateEasing->Update(Frame::DeltaTimeRate());
        }

        ///------------------------------------------------------------------------
        /// カラーイージング更新（RGB）
        ///------------------------------------------------------------------------
        if (it->isAdaptColorEasing) {
            it->colorEasing->Update(Frame::DeltaTimeRate());
        }

        ///------------------------------------------------------------------------
        /// アルファイージング更新
        ///------------------------------------------------------------------------
        if (it->isAdaptAlphaEasing) {
            it->alphaEasing->Update(Frame::DeltaTimeRate());
        }

        ///------------------------------------------------------------------------
        /// Rotateイージング更新
        ///------------------------------------------------------------------------
        if (it->isAdaptRotateEasing) {
            it->rotateEasing->Update(Frame::DeltaTimeRate());
        } else {
            // イージングを使わない場合は通常の回転速度を適用
            it->worldTransform_->rotation_.x += it->rotateSpeed_.x * Frame::DeltaTimeRate();
            it->worldTransform_->rotation_.y += it->rotateSpeed_.y * Frame::DeltaTimeRate();
            it->worldTransform_->rotation_.z += it->rotateSpeed_.z * Frame::DeltaTimeRate();
        }

        ///------------------------------------------------------------------------
        /// 重力の適用
        ///------------------------------------------------------------------------
        it->velocity_.y += it->gravity_ * Frame::DeltaTime();

        ///------------------------------------------------------------------------
        /// 変位更新
        ///------------------------------------------------------------------------
        if (it->followPos) {
            it->worldTransform_->translation_ = *it->followPos + it->offSet;
        } else {
            // Translateイージングを使用していない場合のみ通常の移動を適用
            if (!it->isAdaptTranslateEasing) {
                it->worldTransform_->translation_.y += it->velocity_.y * Frame::DeltaTime();

                if (it->isFloatVelocity) {
                    // 方向ベクトル × スカラー速度
                    it->worldTransform_->translation_ += it->direction_ * it->speed_ * Frame::DeltaTime();
                } else {
                    // ベクトル速度そのまま適用
                    it->worldTransform_->translation_ += it->speedV3 * Frame::DeltaTime();
                }
            }
        }

        ///------------------------------------------------------------------------
        /// UV更新
        ///------------------------------------------------------------------------
        if (it->uvInfo_.numOfFrame > 0) {
            UpdateUV(it->uvInfo_, Frame::DeltaTime());
        }

        ///------------------------------------------------------------------------
        /// ビルボードまたは通常の行列更新
        ///------------------------------------------------------------------------
        if (group.param.isBillboard) {
            it->worldTransform_->BillboardUpdateMatrix(*viewProjection, group.param.billboardType, group.param.adaptRotate_);
        } else {
            it->worldTransform_->UpdateMatrix();
        }

        // 時間を進める
        it->currentTime_ += Frame::DeltaTime();

        ///------------------------------------------------------------------------
        /// Dissolveイージング更新
        ///------------------------------------------------------------------------
        if (it->isAdaptDissolveEasing) {
            if (it->dissolveCurrentTime < it->dissolveOffsetTime) {
                it->dissolveCurrentTime += Frame::DeltaTimeRate();
            } else {
                it->dissolveEasing->Update(Frame::DeltaTimeRate());
            }
        }

        ++it;
    }
}

void ParticleUpdater::UpdateUV(ParticleManager::UVInfo& uvInfo, float deltaTime) {
    if (uvInfo.numOfFrame == 1) {
        // 連続スクロール（ScrollEachPixel相当）
        uvInfo.pos.x += uvInfo.frameScrollSpeed * deltaTime;
    } else {
        // コマ送り制御（スプライトシート）
        uvInfo.currentScrollTime += deltaTime;

        if (uvInfo.currentScrollTime >= uvInfo.frameScrollSpeed) {
            uvInfo.currentScrollTime = 0.0f;
            uvInfo.pos.x += uvInfo.frameDistance_;

            if (!uvInfo.isLoop) {
                uvInfo.pos.x = std::min(uvInfo.pos.x, uvInfo.uvStopPos_);
            }
        }
    }
}
